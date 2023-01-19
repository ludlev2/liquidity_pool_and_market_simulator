#include "SimulationPlayground.h"
#include "ui_SimulationPlayground.h"
#include "PoolGraphItem.h"
#include "AccountGraphItem.h"
#include <QVector>
#include <random>

enum VIEW_METHOD {
    VIEW_QUANTITY,
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
    ui(new Ui::SimulationPlayground)
{
    ui->setupUi(this);
    market_ = new Market;

    QListWidgetItem *item = new QListWidgetItem(ui->listWidget);
    ui->listWidget->addItem(item);

    update_pool_comboBox();
    ui->View_Options->addItem("View Quantity", QVariant::fromValue(VIEW_METHOD::VIEW_QUANTITY));
    ui->View_Options->addItem("View Spot Price", QVariant::fromValue(VIEW_METHOD::VIEW_PRICE));
    ui->View_Options->setCurrentIndex(0);
    QString default_scenario = "{ \"price_tags\": { \"ETH\": 1012, \"DAI\": 10, \"BTC\": 5 }}";
    ui->textEdit_initial_scenario->setText(default_scenario);
}


SimulationPlayground::~SimulationPlayground() {
    delete ui;
}

void SimulationPlayground::on_runButton_clicked() {
    market_->runEpoch();

    if (ui->pool_comboBox->currentIndex() != -1) {
        QListWidgetItem *item = ui->listWidget->item(0);
        QWidget *item_widget = ui->listWidget->itemWidget(item);

        PoolGraphItem *pool_graph = qobject_cast<PoolGraphItem *>(item_widget);

        pool_graph->setViewMethod(qvariant_cast<VIEW_METHOD>(ui->View_Options->currentData()) == VIEW_METHOD::VIEW_QUANTITY);
        pool_graph->UpdateGraph();
    }

    QListWidgetItem *item2 = ui->listWidget2->item(0);
    QWidget *item_widget_2 = ui->listWidget2->itemWidget(item2);
    AccountGraphItem *arbitrager_graph = qobject_cast<AccountGraphItem *>(item_widget_2);
    arbitrager_graph->UpdateGraph();

    QListWidgetItem *item3 = ui->listWidget3->item(0);
    QWidget *item_widget_3 = ui->listWidget3->itemWidget(item3);
    AccountGraphItem *provider_graph = qobject_cast<AccountGraphItem *>(item_widget_3);
    provider_graph->UpdateGraph();
    /*
    if(ui->comboBox->currentIndex() != -1){
        QListWidgetItem *arbitrager = ui->listWidget2->item(0);
        QWidget *arbitrager_widget = ui->listWidget2->itemWidget(arbitrager);
        AccountGraphItem *arbitrager_graph = qobject_cast<AccountGraphItem *>(arbitrager_widget);
        arbitrager_graph->UpdateGraph();
    }
    if(ui->comboBox_2->currentIndex() != -1){
        QListWidgetItem *provider = ui->listWidget3->item(0);
        QWidget *provider_widget = ui->listWidget3->itemWidget(provider);
        AccountGraphItem *provider_graph = qobject_cast<AccountGraphItem *>(provider_widget);
        provider_graph->UpdateGraph();
    }
    */
}

void SimulationPlayground::on_pool_comboBox_currentIndexChanged(int index) {
    if(index == -1)
        return;

    PoolInterface *pool = qvariant_cast<PoolInterface *>(ui->pool_comboBox->itemData(index));
    PoolGraphItem *pool_graph = new PoolGraphItem(this, pool);

    pool_graph->setViewMethod(qvariant_cast<VIEW_METHOD>(ui->View_Options->currentData()) == VIEW_METHOD::VIEW_QUANTITY);
    pool_graph->UpdateGraph();

    QListWidgetItem *item = ui->listWidget->item(0);
    item->setSizeHint(pool_graph->sizeHint());
    ui->listWidget->setItemWidget(item, pool_graph);
}


void SimulationPlayground::on_pushButton_customEpoch_clicked()
{
    if(ui->lineEdit->text() == ""){
        QMessageBox::about(this, "Invalid Epoch Number", "Enter epoch number");
    }
    for(int i = 0; i < ui->lineEdit->text().toInt(); i++){
        on_runButton_clicked();
    }
}



void SimulationPlayground::on_View_Options_currentIndexChanged(int index) {
    if (ui->pool_comboBox->currentIndex() == -1)
        return;

    PoolInterface *pool = qvariant_cast<PoolInterface *>(ui->pool_comboBox->currentData());
    PoolGraphItem *pool_graph = new PoolGraphItem(this, pool);

    pool_graph->setViewMethod(qvariant_cast<VIEW_METHOD>(ui->View_Options->itemData(index)) == VIEW_METHOD::VIEW_QUANTITY);
    pool_graph->UpdateGraph();

    QListWidgetItem *item = ui->listWidget->item(0);
    item->setSizeHint(pool_graph->sizeHint());
    ui->listWidget->setItemWidget(item, pool_graph);
}

void SimulationPlayground::update_pool_comboBox(){
    for (PoolInterface *pool : market_->GetMarketPools()) {
        ui->pool_comboBox->addItem(QString::fromStdString(getPoolName(pool)),
                                   QVariant::fromValue(pool));
    }
}

void SimulationPlayground::on_pushButton_load_scenario_clicked()
{
    QString scenario_string = ui->textEdit_initial_scenario->toPlainText();
    std::unordered_map<std::string, double> price_tags_ = verify_scenario(scenario_string);
    if(price_tags_.empty()){
        return;
    }
    on_pushButton_reset_market_clicked();
    market_->loadInitialScenario(price_tags_, PROTOCOL::UNISWAP_V2);
    update_pool_comboBox();
    ui->pool_comboBox->setCurrentIndex(0);
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

void SimulationPlayground::on_pushButton_reset_market_clicked()
{
    delete market_;
    market_ = new Market();
    ui->pool_comboBox->clear();
    ui->listWidget->clear();
    QListWidgetItem *item = new QListWidgetItem(ui->listWidget);
    ui->listWidget->addItem(item);
}