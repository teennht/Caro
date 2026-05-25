#ifndef CARO_VIET_CORE_SAVE_MANAGER_HPP
#define CARO_VIET_CORE_SAVE_MANAGER_HPP

#include <string>
#include "GameSnapshot.hpp"
#include "SaveError.hpp"

namespace caro {

struct SaveResult {
    SaveError error = SaveError::None;
    std::string errorMessage;
    bool success() const { return error == SaveError::None; }
};

struct LoadResult {
    SaveError error = SaveError::None;
    std::string errorMessage;
    GameSnapshot snapshot;
    bool success() const { return error == SaveError::None; }
};

class ISaveRepository {
public:
    virtual ~ISaveRepository() = default;
    virtual SaveResult save(const GameSnapshot& snapshot, const std::string& path) = 0;
    virtual LoadResult load(const std::string& path) = 0;
};

class SaveManager : public ISaveRepository {
public:
    SaveResult save(const GameSnapshot& snapshot, const std::string& path) override;
    LoadResult load(const std::string& path) override;
};

} // namespace caro

#endif // CARO_VIET_CORE_SAVE_MANAGER_HPP
