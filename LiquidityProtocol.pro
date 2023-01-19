QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets printsupport

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    AccountGraphItem.cpp \
    AccountListWidgetItem.cpp \
    ChangeModeDialog.cpp \
    ExistingPoolProvisionDialog.cpp \
    MintDialog.cpp \
    NewPoolProvisionDialog.cpp \
    PoolGraphItem.cpp \
    PoolListWidgetItem.cpp \
    ProvideDialog.cpp \
    SimulationPlayground.cpp \
    TokenListWidgetItem.cpp \
    TradeDialog.cpp \
    WalletListWidgetItem.cpp \
    WithdrawDialog.cpp \
    main.cpp \
    MainWindow.cpp \
    qcustomplot.cpp \
    src/Market/Agents/Arbitrager.cpp \
    src/Market/Agents/Provider.cpp \
    src/Market/Infrastructure/LP_Profit.cpp \
    src/Market/Infrastructure/Signals.cpp \
    src/Market/Infrastructure/SignalsHandler.cpp \
    src/Market/Simulation.cpp \
    src/Utilities/Account.cpp \
    src/Utilities/Operation.cpp \
    src/Utilities/PoolInterface.cpp \
    src/Utilities/Token.cpp \
    src/Utilities/TokensContainer.cpp \
    src/Protocols/CurvePool.cpp \
    src/Protocols/Protocols.cpp \
    src/Playground.cpp \
    src/Market/Infrastructure/Market.cpp \
    src/Market/Infrastructure/CommunityActor.cpp \
    src/Utilities/Utilities.cpp

HEADERS += \
    AccountGraphItem.h \
    AccountListWidgetItem.h \
    ChangeModeDialog.h \
    ExistingPoolProvisionDialog.h \
    MainWindow.h \
    MintDialog.h \
    NewPoolProvisionDialog.h \
    PoolGraphItem.h \
    PoolListWidgetItem.h \
    ProvideDialog.h \
    SimulationPlayground.h \
    TokenListWidgetItem.h \
    TradeDialog.h \
    WalletListWidgetItem.h \
    qcustomplot.h \
    WithdrawDialog.h \
    src/Market/Agents/Arbitrager.hpp \
    src/Market/Agents/Provider.hpp \
    src/Market/Infrastructure/Signals.hpp \
    src/Market/Infrastructure/SignalsHandler.hpp \
    src/Market/Simulation.hpp \
    src/Market/Strategy/Strat_HashAI.hpp \
    src/Market/Strategy/Strat_LinearRegression.hpp \
    src/Market/Strategy/Strat_MovingAverage.hpp \
    src/Utilities/Utilities.hpp \
    src/Protocols/BalancerPool.hpp \
    src/Protocols/ConstantSum.hpp \
    src/Protocols/CurvePool.hpp \
    src/Protocols/Protocols.hpp \
    src/Protocols/UniswapV2Pool.hpp \
    src/Protocols/UniswapV3Pool.hpp \
    src/Playground.hpp \
    src/Market/Infrastructure/Market.hpp \
    src/Market/Infrastructure/CommunityActor.hpp

FORMS += \
    AccountGraphItem.ui \
    AccountListWidgetItem.ui \
    ChangeModeDialog.ui \
    ExistingPoolProvisionDialog.ui \
    MainWindow.ui \
    MintDialog.ui \
    NewPoolProvisionDialog.ui \
    PoolGraphItem.ui \
    PoolListWidgetItem.ui \
    ProvideDialog.ui \
    SimulationPlayground.ui \
    TokenListWidgetItem.ui \
    TradeDialog.ui \
    WalletListWidgetItem.ui \
    WithdrawDialog.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
