#include "SimulationPlayground.h"
#include "ui_SimulationPlayground.h"
#include "PoolGraphItem.h"
#include "./src/Market/Agents/Arbitrager.hpp"

#include <QVector>
#include <QTimer>
#include <random>

#include "ProviderSimulationGraphItem.h"
#include "ArbitrageurSimulationGraphItem.h"

enum VIEW_METHOD {
    VIEW_VOLUME,
    VIEW_PRICE
};
std::string getPoolName(PoolInterface *pool) {
    std::string name = "";

    for (Token *token : pool->tokens()) {
        name += token->name();
        name += " & ";
    }
    return std::string(name.begin(), name.end() - 3);
}

Q_DECLARE_METATYPE(VIEW_METHOD);

SimulationPlayground::SimulationPlayground(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SimulationPlayground),
    stopped_(false),
    is_running(false)
{
    ui->setupUi(this);
    Sim = new Simulation();

    arbitrageur = Sim->GetArb();
    step = 0;
    wallet_values = { arbitrageur->total_value() };
    epochs = {step};

    ui->tabWidget->clear();
    ui->tabWidget->addTab(new QWidget, "Pool Graph");
    ui->tabWidget->addTab(new QWidget, "LP Graph");
    ui->tabWidget->addTab(new QWidget, "Arbitrageur Graph");
    update_pool_comboBox();

    for(auto &[strategy, strategy_name] : STRATEGY_NAME) {
        ui->Arbs_Options->addItem(strategy_name, QVariant::fromValue(strategy));
    }
    ui->Arbs_Options->setCurrentIndex(0);

    ui->View_Options->addItem("View Volume", QVariant::fromValue(VIEW_METHOD::VIEW_VOLUME));
    ui->View_Options->addItem("View Price", QVariant::fromValue(VIEW_METHOD::VIEW_PRICE));
    ui->View_Options->setCurrentIndex(0);
    QString default_scenario = "{ \"price_tags\": { \"ETH\": 1012, \"DAI\": 10, \"BTC\": 5 }}";
    ui->textEdit_initial_scenario->setText(default_scenario);
}

SimulationPlayground::~SimulationPlayground() {
    delete ui;
}

void SimulationPlayground::on_runButton_clicked() {
    if (Sim->GetPools().empty()) {
        QMessageBox::about(this, "Run failed", "Market has no pool!");
        return;
    }
    Sim->runEpoch();

    step = step + 1;
    epochs.append(step);
    wallet_values.append( arbitrageur->total_value() );

    if (ui->pool_comboBox->currentIndex() != -1) {
        QWidget *item_widget = ui->tabWidget->widget(0);
        QWidget *lp_widget = ui->tabWidget->widget(1);
        QWidget *arb_widget = ui->tabWidget->widget(2);

        PoolGraphItem *pool_graph = qobject_cast<PoolGraphItem *>(item_widget);
        ProviderSimulationGraphItem *lp_graph = qobject_cast<ProviderSimulationGraphItem *>(lp_widget);
        ArbitrageurSimulationGraphItem *arb_graph = qobject_cast<ArbitrageurSimulationGraphItem *>(arb_widget);

        pool_graph->setViewMethod(qvariant_cast<VIEW_METHOD>(ui->View_Options->currentData()) == VIEW_METHOD::VIEW_VOLUME);
        pool_graph->UpdateGraph();
        lp_graph->UpdateGraph();
        arb_graph->UpdateGraph(epochs, wallet_values);
    }
}

void SimulationPlayground::on_pool_comboBox_currentIndexChanged(int index) {
    if(index == -1)
        return;

    PoolInterface *pool = qvariant_cast<PoolInterface *>(ui->pool_comboBox->itemData(index));
    PoolGraphItem *pool_graph = new PoolGraphItem(this, pool);
    ProviderSimulationGraphItem *lp_graph = new ProviderSimulationGraphItem(this, pool, Sim->GetLP());
    ArbitrageurSimulationGraphItem *arb_graph = new ArbitrageurSimulationGraphItem(this, epochs, wallet_values);

    pool_graph->setViewMethod(qvariant_cast<VIEW_METHOD>(ui->View_Options->currentData()) == VIEW_METHOD::VIEW_VOLUME);
    pool_graph->UpdateGraph();
    lp_graph->UpdateGraph();

    ui->tabWidget->clear();
    ui->tabWidget->insertTab(0, pool_graph, "Pool Graph");
    ui->tabWidget->addTab(lp_graph, "LP Graph");
    ui->tabWidget->addTab(arb_graph, "Arb Graph");
    ui->tabWidget->setCurrentIndex(0);
}

void SimulationPlayground::on_pushButton_customEpoch_clicked() {
    if(ui->lineEdit->text() == ""){
        QMessageBox::about(this, "Invalid Epoch Number", "Enter epoch number");
        return;
    }
    if (Sim->GetPools().empty()) {
        QMessageBox::about(this, "Run failed", "Market has no pool!");
        return;
    }
    is_running = true;
    int customEpoch = ui->lineEdit->text().toInt();
    ui->lineEdit->setReadOnly(true);
    while (customEpoch--) {
        if (stopped_) {
            stopped_ = false;
            break;
        }
        ui->lineEdit->setText(QString::fromStdString(std::to_string(customEpoch)));
        on_runButton_clicked();

        QTime dieTime = QTime::currentTime().addMSecs(500);
        while (QTime::currentTime() < dieTime)
            QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
    }
    ui->lineEdit->clear();
    ui->lineEdit->setReadOnly(false);
    is_running = false;
}

void SimulationPlayground::on_View_Options_currentIndexChanged(int index) {
    if (ui->pool_comboBox->currentIndex() == -1)
        return;

    PoolInterface *pool = qvariant_cast<PoolInterface *>(ui->pool_comboBox->currentData());
    PoolGraphItem *pool_graph = new PoolGraphItem(this, pool);

    pool_graph->setViewMethod(qvariant_cast<VIEW_METHOD>(ui->View_Options->itemData(index)) == VIEW_METHOD::VIEW_VOLUME);
    pool_graph->UpdateGraph();

    ui->tabWidget->removeTab(0);
    ui->tabWidget->insertTab(0, pool_graph, "Pool Graph");
    ui->tabWidget->setCurrentIndex(0);
}

void SimulationPlayground::update_pool_comboBox(){
    for (auto pool : Sim->GetPools()) {
        ui->pool_comboBox->addItem(QString::fromStdString(getPoolName(pool)),
                                   QVariant::fromValue(pool));
    }
}

void SimulationPlayground::on_pushButton_load_scenario_clicked() {
    QString scenario_string = ui->textEdit_initial_scenario->toPlainText();
    std::unordered_map<std::string, double> price_tags_ = verify_scenario(scenario_string);
    if (price_tags_.empty()){
        return;
    }
    delete Sim;
    Sim = new Simulation();

    ui->pool_comboBox->clear();
    ui->tabWidget->clear();
    ui->tabWidget->insertTab(0, new QWidget, "Pool Graph");
    ui->tabWidget->addTab(new QWidget, "LP Graph");
    ui->tabWidget->addTab(new QWidget, "Arb Graph");
    ui->tabWidget->setCurrentIndex(0);

    Market *market = Sim->GetMarket();
    market->loadInitialScenario(price_tags_, PROTOCOL::UNISWAP_V2);

    arbitrageur = Sim->GetArb();

    step = 0;
    wallet_values = {10000};
    epochs = {0};

    update_pool_comboBox();
    ui->pool_comboBox->setCurrentIndex(0);

    for (int _ = 0 ; _ < 50 ; ++_)
        on_runButton_clicked();
}

std::unordered_map<std::string, double> SimulationPlayground::verify_scenario(QString scenario){
    QByteArray byte_arr = scenario.toUtf8();
    QJsonDocument doc = QJsonDocument::fromJson(byte_arr);
    QJsonObject object = doc.object();
    QJsonObject price_tags_object = object["price_tags"].toObject();
    QStringList tokens = price_tags_object.keys();
    if (object["price_tags"].toObject().isEmpty()){
        QMessageBox::about(this, "Set scenario failed", "Invalid JSON file");
        return {};
    }
    std::unordered_map<std::string, double> price_tags_;
    for(auto token: tokens){
        double price = price_tags_object[token].toDouble();
        price_tags_[token.toStdString()] = price;
    }
    return price_tags_;

}

void SimulationPlayground::on_pushButton_reset_market_clicked() {
    if (is_running)
        stopped_ = true;

    on_pushButton_load_scenario_clicked();
}


void SimulationPlayground::on_Arbs_Options_currentIndexChanged(int index) {
    auto arb = Sim->GetArb();
    arb->setStrategy(qvariant_cast<STRATEGY>(ui->Arbs_Options->itemData(index)));
}

void SimulationPlayground::on_pushButton_stop_clicked()
{
    if (is_running)
        stopped_ = true;
}
