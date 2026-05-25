#ifndef CARO_VIET_CORE_SAVE_ERROR_HPP
#define CARO_VIET_CORE_SAVE_ERROR_HPP

namespace caro {

enum class SaveError {
    None,
    FileNotFound,
    PermissionDenied,
    InvalidFormat,
    UnsupportedVersion,
    InvalidBoardSize,
    InvalidMode,
    InvalidPlayer,
    InvalidTurn,
    InvalidMove,
    InvalidDifficulty,
    CorruptedData
};

} // namespace caro

#endif // CARO_VIET_CORE_SAVE_ERROR_HPP
