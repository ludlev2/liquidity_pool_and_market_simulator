#ifndef UTILITIES_HPP
#define UTILITIES_HPP

#include <unordered_map>
#include <unordered_set>
#include <string>
#include <stdexcept>
#include <algorithm>
#include <vector>
#include <iostream>
#include <utility>
#include <QVariant>

#define UNUSED(x) (void)(x)

template<typename T1, typename T2>
std::unordered_set<T1> GetKeys(const std::unordered_map<T1, T2> &mp);

namespace Utilities {
bool ValidNumber(std::string text);
}

class PoolInterface;

class Token {
public:
	// disallow copying
	Token & operator=(const Token &) = delete;
	Token(const Token &) = delete;

    friend class PoolInterface;
    friend class Playground;
    friend class Market;

	std::string name() const;
	double real_value() const;
    PoolInterface * pool() const;

private:
	std::string name_;
	double real_value_;
    PoolInterface *pool_;
    
	Token(const std::string &name, double real_value);
    Token(PoolInterface *pool);
};

class TokensContainer {
public:
    TokensContainer(const std::unordered_set<Token *> &tokens);

    std::vector<Token *> tokens() const;

    bool operator==(const TokensContainer &other) const;

private:
    std::vector<Token *> tokens_;
};

namespace std {
    template<>
    struct hash<TokensContainer> {
        size_t operator()(const TokensContainer &a) const {
            string temp = "";
            for (auto token : a.tokens()) {
                temp += " " + token->name();
            }
            return hash<string>()(temp);
        }
    };
}

class Operation {
public:
    Operation(
        const std::string &operation_type,
        const std::string &account_name,
        PoolInterface *pool,
        const std::unordered_map<Token *, double> &input,
        const std::unordered_map<Token *, double> &output
    );

    std::string operation_type() const;
    std::string account_name() const;
    PoolInterface *pool() const;
    std::unordered_map<Token *, double> input() const;
    std::unordered_map<Token *, double> output() const;

    double GetMarketPrice(Token *a) const;
    double GetSpotPrice(Token *a, Token *b) const;
    double GetQuanitty(Token *a) const;
    double GetVolume(Token *a) const;

    friend std::ostream & operator<<(std::ostream &os, const Operation &op);
private:
    std::string operation_type_;
    std::string account_name_;
    PoolInterface *pool_;
    std::unordered_map<Token *, double> quantities_;
    std::unordered_map<Token *, double> input_;
    std::unordered_map<Token *, double> output_;
    std::unordered_map<Token *, double> market_price_;

    std::unordered_map<Token *, std::unordered_map<Token *, double> > spotPriceMatrix;
};

class PoolEpochData {
public:
    PoolEpochData(PoolInterface *pool);

    double GetOpenPrice(Token *a, Token *b) const;
    double GetHighPrice(Token *a, Token *b) const;
    double GetLowPrice(Token *a, Token *b) const;
    double GetClosePrice(Token *a, Token *b) const;

    double GetMarketPrice(Token *a) const;
    double GetSpotPrice(Token *a, Token *b) const;
    double GetQuanitty(Token *a) const;
    double GetVolume(Token *a) const;

    PoolInterface *pool() const;
    int epochIndex() const;

    friend class PoolInterface;
private:
    std::unordered_map<Token *, std::unordered_map<Token *, double> > open, high, low, close;

    std::unordered_map<Token *, double> quantities;
    std::unordered_map<Token *, double> market_price;

    PoolInterface *pool_;
    int nEpoch;
};

class Account {
public:
    Account(const std::string &name);
    Account(const std::string &name, double budget);

    Account & operator=(const Account &) = delete;
	Account(const Account &) = delete;

    friend class PoolInterface;
    friend class Playground;
    friend class SignalsHandler;

    std::string name() const;
    std::unordered_map<Token *, double> wallet() const;

    double total_value() const;
    double budget() const;
    double total_asset() const;
    std::vector<Operation *> ledger() const;

    double GetQuantity(Token *token) const;

    void Deposit(Token *token, double quantity);

    void sell(Token *token, double quantity);
    void buy(Token *token, double quantity);
protected:
    std::string name_;
    std::unordered_map<Token *, double> wallet_;
    double budget_;
    std::vector<Operation *> ledger_;

    double Trade(PoolInterface *pool, Token *input_token, Token *output_token, double input_quantity);
    double Provide(PoolInterface *pool, std::unordered_map<Token *, double> provided_quantities);
    std::unordered_map<Token *, double> Withdraw(PoolInterface *pool, double surrendered_quantity);
};

class PoolInterface {
public:
    PoolInterface & operator=(const PoolInterface &) = delete;
	PoolInterface(const PoolInterface &) = delete;

    friend class Account;
    friend class Playground;
    friend class CommunityActor;
    friend class Market;
    friend class Signal;

    std::string name() const;

    bool InPool(Token *token) const;
    double GetQuantity(Token *token) const;

    double pool_fee() const;

    Token * pool_token() const;
    double total_pool_token_quantity() const;

    std::unordered_set<Token *> tokens() const;
    std::unordered_map<Token *, double> quantities() const;

    double GetSlippage(Token *input_token, Token *output_token, double input_quantity) const;
    double GetSpotPrice(Token *input_token, Token *output_token) const;

    std::vector<Operation *> GetLatestOps(int n) const;
    std::vector<Operation *> ledger() const;

    Operation *kthLastOps(int k) const;
    Operation *kthFirstOps(int k) const;

    std::vector<PoolEpochData *> GetLastestEpochs(int n) const;
    std::vector<PoolEpochData *> GetEpochHistory() const;

    PoolEpochData *kthLastEpoch(int k) const;
    PoolEpochData *kthFirstEpoch(int k) const;

    void endEpoch();

    double GetPoolValue() const;

protected:
    static constexpr double INITIAL_POOL_TOKEN_SUPPLY = 100;

    PoolInterface(std::unordered_set<Token *> tokens, double pool_fee);
    PoolInterface(std::unordered_map<Token *, double> quantities, double pool_fee);

    virtual ~PoolInterface() = default;

    virtual double ComputeSwappedQuantity(Token *input_token, Token *output_token, double input_quantity) const = 0;
    virtual double ComputeInvariant(const std::unordered_map<Token *, double> &quantities) const = 0;
    virtual double ComputeSpotExchangeRate(Token *input_token, Token *output_token) const;
    virtual double ComputeSlippage(Token *input_token, Token *output_token, double input_quantity) const;

private:
    TokensContainer tokens_container_;
    std::unordered_map<Token *, double> quantities_;
    double pool_fee_;
    Token *pool_token_;
    std::vector<Operation *> ledger_;
    std::vector<PoolEpochData *> history_;
    size_t lastEpochIndex = 0;

    double SimulateSwap(Token *input_token, Token *output_token, double input_quantity) const;
    Operation * Swap(Account *trader, Token *input_token, Token *output_token, double input_quantity);

    double SimulateProvision(std::unordered_map<Token *, double> input_quantities) const;
    Operation * Provide(Account *provider, std::unordered_map<Token *, double> input_quantities);

    std::unordered_map<Token *, double> SimulateWithdrawal(double surrendered_pool_token_quantity) const;
    Operation * Withdraw(Account *provider, double surrendered_pool_token_quantity);

    bool CheckWallet(Account *account, const std::unordered_map<Token *, double> &quantities) const;

    void UpdateWallet(Account *account, Token *token, double quantity) const;

    void ExecuteSwap(Account *trader, Token *input_token, Token *output_token, double input_quantity, double output_quantity);

    bool ValidProvision(std::unordered_map<Token *, double> quantities) const;

    void ExecuteProvision(Account *provider, std::unordered_map<Token *, double> input_quantities, double generated_pool_token_quantity);

    void ExecuteWithdrawal(Account *provider, double surrendered_pool_token_quantity, std::unordered_map<Token *, double> output_quantities);
};

Q_DECLARE_METATYPE(PoolInterface *);
Q_DECLARE_METATYPE(Token *);

#endif // UTILITIES_HPP
