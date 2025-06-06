/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "napi_async_work.h"
#include "napi_utils.h"
#include "avsession_trace.h"
#include "napi_avsession_manager.h"
#include "avsession_errors.h"

namespace OHOS::AVSession {
ContextBase::~ContextBase()
{
    SLOGD("no memory leak after callback or promise[resolved/rejected]");
    if (env != nullptr) {
        if (work != nullptr) {
            napi_delete_async_work(env, work);
        }
        if (callbackRef != nullptr) {
            napi_delete_reference(env, callbackRef);
        }
        napi_delete_reference(env, selfRef);
        env = nullptr;
        callbackRef = nullptr;
        selfRef = nullptr;
    }
}

void ContextBase::GetCbInfo(napi_env envi, napi_callback_info info, NapiCbInfoParser parser, bool sync)
{
    env = envi;
    size_t argc = ARGC_MAX;
    napi_value argv[ARGC_MAX] = {nullptr};
    status = napi_get_cb_info(env, info, &argc, argv, &self, nullptr);
    CHECK_STATUS_RETURN_VOID(this, "napi_get_cb_info failed!", NapiAVSessionManager::errcode_[AVSESSION_ERROR]);
    CHECK_ARGS_RETURN_VOID(this, argc <= ARGC_MAX, "too many arguments!",
        NapiAVSessionManager::errcode_[ERR_INVALID_PARAM]);
    CHECK_ARGS_RETURN_VOID(this, self != nullptr, "no JavaScript this argument!",
        NapiAVSessionManager::errcode_[ERR_INVALID_PARAM]);
    napi_create_reference(env, self, 1, &selfRef);
    status = napi_unwrap(env, self, &native);
    CHECK_STATUS_RETURN_VOID(this, "self unwrap failed!", NapiAVSessionManager::errcode_[AVSESSION_ERROR]);

    if (!sync && (argc > 0)) {
        // get the last arguments :: <callback>
        size_t index = argc - 1;
        napi_valuetype type = napi_undefined;
        napi_status tyst = napi_typeof(env, argv[index], &type);
        if ((tyst == napi_ok) && (type == napi_function)) {
            status = napi_create_reference(env, argv[index], 1, &callbackRef);
            CHECK_STATUS_RETURN_VOID(this, "ref callback failed!", NapiAVSessionManager::errcode_[AVSESSION_ERROR]);
            argc = index;
            SLOGD("async callback, no promise");
        } else {
            SLOGD("no callback, async promise");
        }
    }

    if (parser) {
        parser(argc, argv);
    } else {
        CHECK_ARGS_RETURN_VOID(this, argc == 0, "required no arguments!",
            NapiAVSessionManager::errcode_[ERR_INVALID_PARAM]);
    }
}

napi_value NapiAsyncWork::Enqueue(napi_env env, std::shared_ptr<ContextBase> ctxt, const std::string& name,
                                  NapiAsyncExecute execute, NapiAsyncComplete complete)
{
    SLOGI("name=%{public}s", name.c_str());
    ctxt->execute = std::move(execute);
    ctxt->complete = std::move(complete);
    ctxt->taskName = name;
    napi_value promise = nullptr;
    if (ctxt->callbackRef == nullptr) {
        if (napi_create_promise(ctxt->env, &ctxt->deferred, &promise) != napi_ok) {
            NapiUtils::ThrowError(env, "napi_create_promise failed",
                NapiAVSessionManager::errcode_[ERR_INVALID_PARAM]);
            return NapiUtils::GetUndefinedValue(env);
        }
        SLOGD("create deferred promise");
    } else {
        napi_get_undefined(ctxt->env, &promise);
    }

    napi_value resource = nullptr;
    napi_create_string_utf8(ctxt->env, name.c_str(), NAPI_AUTO_LENGTH, &resource);
    napi_create_async_work(
        ctxt->env, nullptr, resource,
        [](napi_env env, void* data) {
            CHECK_RETURN_VOID(data != nullptr, "napi_async_execute_callback nullptr");
            auto ctxt = reinterpret_cast<ContextBase*>(data);
            if (!ctxt->taskName.empty() && ctxt->taskId > INVALID_TASK_ID) {
                AVSESSION_TRACE_ASYNC_START("NapiAsyncWork::" + ctxt->taskName, ctxt->taskId);
            }
            if (ctxt->execute && ctxt->status == napi_ok) {
                ctxt->execute();
            }
        },
        [](napi_env env, napi_status status, void* data) {
            CHECK_RETURN_VOID(data != nullptr, "napi_async_complete_callback nullptr");
            auto ctxt = reinterpret_cast<ContextBase*>(data);
            if ((status != napi_ok) && (ctxt->status == napi_ok)) {
                ctxt->status = status;
            }
            if ((ctxt->complete) && (status == napi_ok) && (ctxt->status == napi_ok)) {
                ctxt->complete(ctxt->output);
            }
            if (!ctxt->taskName.empty() && ctxt->taskId > INVALID_TASK_ID) {
                AVSESSION_TRACE_ASYNC_END("NapiAsyncWork::" + ctxt->taskName, ctxt->taskId);
            }
            GenerateOutput(ctxt);
        },
        reinterpret_cast<void*>(ctxt.get()), &ctxt->work);
    napi_queue_async_work_with_qos(ctxt->env, ctxt->work, napi_qos_user_initiated);
    ctxt->hold = ctxt; // save crossing-thread ctxt.
    return promise;
}

void NapiAsyncWork::GenerateOutput(ContextBase* ctxt)
{
    napi_value result[RESULT_ALL] = {nullptr};
    if (ctxt->status == napi_ok) {
        napi_get_undefined(ctxt->env, &result[RESULT_ERROR]);
        if (ctxt->output == nullptr) {
            napi_get_undefined(ctxt->env, &ctxt->output);
        }
        result[RESULT_DATA] = ctxt->output;
    } else {
        napi_value message = nullptr;
        napi_value code = nullptr;
        napi_create_string_utf8(ctxt->env, ctxt->errMessage.c_str(), NAPI_AUTO_LENGTH, &message);
        napi_create_error(ctxt->env, nullptr, message, &result[RESULT_ERROR]);
        napi_create_int32(ctxt->env, ctxt->errCode, &code);
        napi_set_named_property(ctxt->env, result[RESULT_ERROR], "code", code);
        napi_get_undefined(ctxt->env, &result[RESULT_DATA]);
    }
    if (ctxt->deferred != nullptr) {
        if (ctxt->status == napi_ok) {
            SLOGD("deferred promise resolved");
            napi_resolve_deferred(ctxt->env, ctxt->deferred, result[RESULT_DATA]);
        } else {
            SLOGD("deferred promise rejected");
            napi_reject_deferred(ctxt->env, ctxt->deferred, result[RESULT_ERROR]);
        }
    } else {
        napi_value callback = nullptr;
        napi_get_reference_value(ctxt->env, ctxt->callbackRef, &callback);
        napi_value callbackResult = nullptr;
        SLOGD("call callback function");
        napi_call_function(ctxt->env, nullptr, callback, RESULT_ALL, result, &callbackResult);
    }
    if (ctxt->callbackRef != nullptr) {
        SLOGD("do clear callback here for leak bef reset");
        napi_delete_reference(ctxt->env, ctxt->callbackRef);
        ctxt->callbackRef = nullptr;
    }
    if (ctxt->complete != nullptr) {
        SLOGD("do clear complete");
        ctxt->complete = nullptr;
    }
    if (ctxt->execute != nullptr) {
        SLOGD("do clear execute");
        ctxt->execute = nullptr;
    }
    ctxt->hold.reset(); // release ctxt.
}
}