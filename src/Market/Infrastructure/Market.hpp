#ifndef Market_hpp
#define Market_hpp

#include "../../Protocols/Protocols.hpp"

#include "CommunityActor.hpp"
#include "SignalsHandler.hpp"

class Market {
public:
    Market();
    ~Market();

    // the add functions is not used in market simulating process
    // instead is used to create Rug Pull Strategy of Arbitrager
    void addToken(const std::string &name, double price);
    void addToken(Token *token);
    void addPool(PoolInterface *pool);

    bool havePool(PoolInterface *pool) const;
    bool haveToken(Token *token) const;

    void runEpoch();
    void setHandler(SignalsHandler *handler);

    std::unordered_set<Token *> GetMarketTokens() const;
    std::unordered_set<PoolInterface *> GetMarketPools() const;
    Token* getToken(std::string name) const;

    void loadInitialScenario(const std::unordered_map<std::string, double> &price_tags, PROTOCOL pool_type = PROTOCOL::UNISWAP_V2);
    void loadInitialScenario(const std::unordered_set<PoolInterface *> pools);
private:
    static int MarketCount;
    int MarketIndex;

    std::unordered_set<Token *> tokens_;
    std::unordered_set<PoolInterface *> pools_;
    std::unordered_set<Account *> accounts_;

    CommunityActor *A;
    SignalsHandler *handler = new SignalsHandler();

    void simulateTrade(PoolInterface *pool);
    void simulateArbitrage(PoolInterface *pool);
    void simulateProvide(PoolInterface *pool);
};

#endif
