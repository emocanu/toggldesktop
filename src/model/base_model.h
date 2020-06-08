// Copyright 2014 Toggl Desktop developers.

#ifndef SRC_BASE_MODEL_H_
#define SRC_BASE_MODEL_H_

#include <string>
#include <vector>
#include <cstring>
#include <ctime>

#include <json/json.h>  // NOLINT

#include "const.h"
#include "types.h"
#include "util/logger.h"
#include "util/property.h"

#include <Poco/Types.h>

namespace toggl {

class BatchUpdateResult;

class TOGGL_INTERNAL_EXPORT BaseModel {
 public:
    BaseModel() {}
    virtual ~BaseModel() {}

    Property<Poco::Int64> LocalID { 0 };
    Property<Poco::UInt64> ID { 0 };
    Property<guid> GUID { "" };
    Property<Poco::Int64> UIModifiedAt { 0 };
    Property<Poco::UInt64> UID { 0 };
    Property<bool> Dirty { false };
    Property<Poco::Int64> DeletedAt { 0 };
    Property<bool> IsMarkedAsDeletedOnServer { false };
    Property<Poco::Int64> UpdatedAt { 0 };

    // If model push to backend results in an error,
    // the error is attached to the model for later inspection.
    Property<std::string> ValidationError { "" };

    // Flag is set only when sync fails.
    // Its for viewing purposes only. It should not
    // be used to check if a model needs to be
    // pushed to backend. It only means that some
    // attempt to push failed somewhere.
    Property<bool> Unsynced { false };

    void SetLocalID(Poco::Int64 value) {
        LocalID.Set(value);
    }
    void SetID(Poco::UInt64 value);
    void SetUIModifiedAt(Poco::Int64 value);
    void SetUIModified() {
        SetUIModifiedAt(time(nullptr));
    }

    void SetGUID(const std::string &value);
    void SetUID(Poco::UInt64 value);

    void SetDirty();
    void ClearDirty();

    void SetUnsynced();
    void ClearUnsynced();

    // Deleting a time entry hides it from
    // UI and flags it for removal from server:
    void SetDeletedAt(Poco::Int64 value);

    void SetUpdatedAt(Poco::Int64 value);

    std::string UpdatedAtString() const;
    void SetUpdatedAtString(const std::string &value);

    // When a model is deleted
    // on server, it will be removed from local
    // DB using this flag:
    void MarkAsDeletedOnServer();

    bool NeedsPush() const;
    bool NeedsPOST() const;
    bool NeedsPUT() const;
    bool NeedsDELETE() const;

    bool NeedsToBeSaved() const;

    void EnsureGUID();

    void ClearValidationError();
    void SetValidationError(const std::string &value);

    virtual std::string String() const = 0;
    virtual std::string ModelName() const = 0;
    virtual std::string ModelURL() const = 0;

    virtual void LoadFromJSON(Json::Value value) {}
    virtual Json::Value SaveToJSON(int apiVersion = 8) const {
        return 0;
    }
    virtual std::string SyncType() const;
    virtual Json::Value SyncMetadata() const { return {}; }
    virtual Json::Value SyncPayload() const { return {}; }

    virtual bool DuplicateResource(const toggl::error &err) const {
        return false;
    }
    virtual bool ResourceCannotBeCreated(const toggl::error &err) const {
        return false;
    }
    virtual bool ResolveError(const toggl::error &err) {
        return false;
    }

    error LoadFromDataString(const std::string &);

    void Delete();

    error ApplyBatchUpdateResult(BatchUpdateResult * const);

    // Convert model JSON into batch update format.
    error BatchUpdateJSON(Json::Value *result) const;

 protected:
    Logger logger() const;

    bool userCannotAccessWorkspace(const toggl::error &err) const;

 private:
    std::string batchUpdateRelativeURL() const;
    std::string batchUpdateMethod() const;
};

}  // namespace toggl

#endif  // SRC_BASE_MODEL_H_
