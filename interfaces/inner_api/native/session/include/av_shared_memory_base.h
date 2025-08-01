/*
 * Copyright (C) 2025 Huawei Device Co., Ltd.
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

#ifndef AV_SHARED_MEMORY_BASE_H
#define AV_SHARED_MEMORY_BASE_H

#include <string>
#include "av_shared_memory.h"
#include "nocopyable.h"
#include "iremote_proxy.h"

namespace OHOS {
namespace AVSession {
class __attribute__((visibility("default"))) AVSharedMemoryBase :
    public AVSharedMemory, public NoCopyable, public Parcelable {
public:
    /**
     * @brief Construct a new AVSharedMemoryBase object. This function should only be used in the
     * local process.
     *
     * @param size the memory's size, bytes.
     * @param flags the memory's accessible flags, refer to {@AVSharedMemoryBase::Flags}.
     * @param name the debug string
     */
    static std::shared_ptr<AVSharedMemoryBase> CreateFromLocal(int32_t size, uint32_t flags, const std::string &name);

    /**
     * @brief Construct a new AVSharedMemoryBase object. This function should only be used in the
     * remote process.
     *
     * @param fd the memory's fdInit()
     * @param size the memory's size, bytes.
     * @param flags the memory's accessible flags, refer to {@AVSharedMemoryBase::Flags}.
     * @param name the debug string
     */
    static std::shared_ptr<AVSharedMemoryBase> CreateFromRemote(int32_t fd, int32_t size, uint32_t flags,
                                                            const std::string &name);

    ~AVSharedMemoryBase();

    /**
     * @brief Construct a new AVSharedMemoryBase object. This function should only be used in the
     * local process.
     *
     * @param size the memory's size, bytes.
     * @param flags the memory's accessible flags, refer to {@AVSharedMemoryBase::Flags}.
     * @param name the debug string
     */
    AVSharedMemoryBase(int32_t size, uint32_t flags, const std::string &name);

    /**
     * @brief Construct a new AVSharedMemoryBase object. This function should only be used in the
     * local process.
     */
    AVSharedMemoryBase() {};

    /**
     * @brief Intialize the memory. Call this interface firstly before the other interface.
     * @param isMapVirAddr the memory's map virtual address flag, the default value is equal to true.
     * @return Status::OK if success, otherwise the errcode.
     */
    int32_t Init(bool isMapVirAddr = true);

    /**
     * @brief Get the memory's fd, which only valid when the underlying memory
     * chunk is allocated through the ashmem.
     * @return the memory's fd if the memory is allocated through the ashmem, otherwise -1.
     */
    int32_t GetFd() const
    {
        return fd_;
    }

    std::string GetName() const
    {
        return name_;
    }

    int32_t Write(const uint8_t *in, int32_t writeSize, int32_t position = INVALID_POSITION);

    int32_t Read(uint8_t *out, int32_t readSize, int32_t position = INVALID_POSITION);

    int32_t GetUsedSize() const;

    void ClearUsedSize();

    /**
     * @brief Get the memory's virtual address
     * @return the memory's virtual address if the memory is valid, otherwise nullptr.
     */
    virtual uint8_t *GetBase() const override
    {
        return base_;
    }

    /**
     * @brief Get the memory's size
     * @return the memory's size if the memory is valid, otherwise -1.
     */
    virtual int32_t GetSize() const override
    {
        return (base_ != nullptr) ? capacity_ : -1;
    }

    /**
     * @brief Get the memory's flags set by the creator, refer to {@Flags}
     * @return the memory's flags if the memory is valid, otherwise 0.
     */
    virtual uint32_t GetFlags() const final
    {
        return (base_ != nullptr) ? flags_ : 0;
    }

    bool Marshalling(Parcel& out) const override;
    bool WriteToParcel(MessageParcel& out) const;
    bool ReadFromParcel(MessageParcel& in);
    static AVSharedMemoryBase* Unmarshalling(Parcel& in);

protected:
    AVSharedMemoryBase(int32_t fd, int32_t size, uint32_t flags, const std::string &name);

private:
    int32_t MapMemory(bool isRemote);
    void Close() noexcept;

    uint8_t *base_ = nullptr;
    int32_t capacity_ = 0;
    uint32_t flags_ = 0;
    std::string name_;
    int32_t fd_ = 0;
    int32_t size_ = 0;
    static constexpr int32_t INVALID_POSITION = -1;
};
} // namespace AVSession
} // namespace OHOS

#endif // AV_SHARED_MEMORY_BASE_H