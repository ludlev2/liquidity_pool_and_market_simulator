#include "Utilities.hpp"
#include "../Protocols/UniswapV2Pool.hpp"

// existing tokens are stored in a static variable of the Token class to avoid creating multiple instances of the same token
std::unordered_map<std::string, Token *> Token::existing_tokens_ = std::unordered_map<std::string, Token *>();

// existing accounts are stored in a static variable of the Token class to avoid creating multiple instances of the same token
std::unordered_map<std::string, Account *> Account::existing_accounts_ = std::unordered_map<std::string, Account *>();

std::unordered_map<TokensContainer, UniswapV2Pool *> UniswapV2Pool::existing_pools_ = std::unordered_map<TokensContainer, UniswapV2Pool *>();
