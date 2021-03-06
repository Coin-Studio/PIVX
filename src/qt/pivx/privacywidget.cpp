// Copyright (c) 2019-2020 The PIVX developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "qt/pivx/privacywidget.h"
#include "qt/pivx/forms/ui_privacywidget.h"
#include "qt/pivx/qtutils.h"
#include "guiutil.h"
#include "qt/pivx/txviewholder.h"
#include "walletmodel.h"
#include "optionsmodel.h"
#include "coincontroldialog.h"
#include "coincontrol.h"

#define DECORATION_SIZE 65
#define NUM_ITEMS 3

PrivacyWidget::PrivacyWidget(PIVXGUI* parent) :
    PWidget(parent),
    ui(new Ui::PrivacyWidget)
{
    ui->setupUi(this);
    this->setStyleSheet(parent->styleSheet());

    /* Containers */
    setCssProperty(ui->left, "container");
    ui->left->setContentsMargins(0,20,0,0);
    setCssProperty(ui->right, "container-right");
    ui->right->setContentsMargins(20,10,20,20);

    /* Light Font */
    QFont fontLight;
    fontLight.setWeight(QFont::Light);

    /* Title */
    ui->labelTitle->setText(tr("Privacy"));
    setCssTitleScreen(ui->labelTitle);
    ui->labelTitle->setFont(fontLight);

    /* Button Group */
    ui->pushLeft->setText(tr("Convert"));
    setCssProperty(ui->pushLeft, "btn-check-left");
    ui->pushRight->setText(tr("Mint"));
    setCssProperty(ui->pushRight, "btn-check-right");

    /* Subtitle */
    ui->labelSubtitle1->setText(tr("Minting zPIV anonymizes your PIV by removing any\ntransaction history, making transactions untraceable "));
    setCssSubtitleScreen(ui->labelSubtitle1);

    ui->labelSubtitle2->setText(tr("Mint new zPIV or convert back to PIV"));
    setCssSubtitleScreen(ui->labelSubtitle2);
    ui->labelSubtitle2->setContentsMargins(0,2,0,0);
    setCssProperty(ui->labelSubtitleAmount, "text-title");

    ui->lineEditAmount->setPlaceholderText("0.00 PIV ");
    ui->lineEditAmount->setValidator(new QRegExpValidator(QRegExp("[0-9]+")));
    initCssEditLine(ui->lineEditAmount);

    /* Denom */
    ui->labelTitleDenom1->setText("Denom. with value 1:");
    setCssProperty(ui->labelTitleDenom1, "text-subtitle");
    ui->labelValueDenom1->setText("0x1 = 0 zPIV");
    setCssProperty(ui->labelValueDenom1, "text-body2");

    ui->labelTitleDenom5->setText("Denom. with value 5:");
    setCssProperty(ui->labelTitleDenom5, "text-subtitle");
    ui->labelValueDenom5->setText("0x5 = 0 zPIV");
    setCssProperty(ui->labelValueDenom5, "text-body2");

    ui->labelTitleDenom10->setText("Denom. with value 10:");
    setCssProperty(ui->labelTitleDenom10, "text-subtitle");
    ui->labelValueDenom10->setText("0x10 = 0 zPIV");
    setCssProperty(ui->labelValueDenom10, "text-body2");

    ui->labelTitleDenom50->setText("Denom. with value 50:");
    setCssProperty(ui->labelTitleDenom50, "text-subtitle");
    ui->labelValueDenom50->setText("0x50 = 0 zPIV");
    setCssProperty(ui->labelValueDenom50, "text-body2");

    ui->labelTitleDenom100->setText("Denom. with value 100:");
    setCssProperty(ui->labelTitleDenom100, "text-subtitle");
    ui->labelValueDenom100->setText("0x100 = 0 zPIV");
    setCssProperty(ui->labelValueDenom100, "text-body2");

    ui->labelTitleDenom500->setText("Denom. with value 500:");
    setCssProperty(ui->labelTitleDenom500, "text-subtitle");
    ui->labelValueDenom500->setText("0x500 = 0 zPIV");
    setCssProperty(ui->labelValueDenom500, "text-body2");

    ui->labelTitleDenom1000->setText("Denom. with value 1000:");
    setCssProperty(ui->labelTitleDenom1000, "text-subtitle");
    ui->labelValueDenom1000->setText("0x1000 = 0 zPIV");
    setCssProperty(ui->labelValueDenom1000, "text-body2");

    ui->labelTitleDenom5000->setText("Denom. with value 5000:");
    setCssProperty(ui->labelTitleDenom5000, "text-subtitle");
    ui->labelValueDenom5000->setText("0x5000 = 0 zPIV");
    setCssProperty(ui->labelValueDenom5000, "text-body2");

    ui->layoutDenom->setVisible(false);

    // List
    ui->labelListHistory->setText(tr("Last zPIV Movements"));
    setCssProperty(ui->labelListHistory, "text-title");

    //ui->emptyContainer->setVisible(false);
    setCssProperty(ui->pushImgEmpty, "img-empty-privacy");
    ui->labelEmpty->setText(tr("No transactions yet"));
    setCssProperty(ui->labelEmpty, "text-empty");

    // Buttons
    setCssBtnPrimary(ui->pushButtonSave);

    // Only Convert to PIV enabled.
    ui->containerViewPrivacyChecks->setVisible(false);
    onMintSelected(false);

    ui->btnTotalzPIV->setTitleClassAndText("btn-title-grey", "Total 0 zPIV");
    ui->btnTotalzPIV->setSubTitleClassAndText("text-subtitle", "Show denominations of zPIV owned.");
    ui->btnTotalzPIV->setRightIconClass("ic-arrow");

    ui->btnCoinControl->setTitleClassAndText("btn-title-grey", "Coin Control");
    ui->btnCoinControl->setSubTitleClassAndText("text-subtitle", "Select PIV outputs to mint into zPIV.");

    ui->btnRescanMints->setTitleClassAndText("btn-title-grey", "Rescan Mints");
    ui->btnRescanMints->setSubTitleClassAndText("text-subtitle", "Find mints in the blockchain.");

    ui->btnResetZerocoin->setTitleClassAndText("btn-title-grey", "Reset Spent zPIV");
    ui->btnResetZerocoin->setSubTitleClassAndText("text-subtitle", "Reset zerocoin database.");

    connect(ui->btnTotalzPIV, &OptionButton::clicked, this, &PrivacyWidget::onTotalZpivClicked);
    connect(ui->btnCoinControl, &OptionButton::clicked, this, &PrivacyWidget::onCoinControlClicked);
    connect(ui->btnRescanMints, &OptionButton::clicked, this, &PrivacyWidget::onRescanMintsClicked);
    connect(ui->btnResetZerocoin, &OptionButton::clicked, this, &PrivacyWidget::onResetZeroClicked);

    ui->pushRight->setChecked(true);
    connect(ui->pushLeft, &QPushButton::clicked, [this](){onMintSelected(false);});
    connect(ui->pushRight,  &QPushButton::clicked, [this](){onMintSelected(true);});

    // List
    setCssProperty(ui->listView, "container");
    txHolder = new TxViewHolder(isLightTheme());
    delegate = new FurAbstractListItemDelegate(
                DECORATION_SIZE,
                txHolder,
                this
    );

    ui->listView->setItemDelegate(delegate);
    ui->listView->setIconSize(QSize(DECORATION_SIZE, DECORATION_SIZE));
    ui->listView->setMinimumHeight(NUM_ITEMS * (DECORATION_SIZE + 2));
    ui->listView->setAttribute(Qt::WA_MacShowFocusRect, false);
    ui->listView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->listView->setLayoutMode(QListView::LayoutMode::Batched);
    ui->listView->setBatchSize(30);
    ui->listView->setUniformItemSizes(true);
}

void PrivacyWidget::loadWalletModel()
{
    if (walletModel) {
        txModel = walletModel->getTransactionTableModel();
        // Set up transaction list
        filter = new TransactionFilterProxy();
        filter->setDynamicSortFilter(true);
        filter->setSortCaseSensitivity(Qt::CaseInsensitive);
        filter->setFilterCaseSensitivity(Qt::CaseInsensitive);
        filter->setSortRole(Qt::EditRole);
        filter->setShowZcTxes(true);
        filter->setSourceModel(txModel);
        filter->sort(TransactionTableModel::Date, Qt::DescendingOrder);
        txHolder->setDisplayUnit(walletModel->getOptionsModel()->getDisplayUnit());
        txHolder->setFilter(filter);
        ui->listView->setModel(filter);

        updateDisplayUnit();
        updateDenomsSupply();

        if (!txModel->hasZcTxes()) {
            ui->emptyContainer->setVisible(true);
            ui->listView->setVisible(false);
        } else {
            showList();
        }

        connect(ui->pushButtonSave, &QPushButton::clicked, this, &PrivacyWidget::onSendClicked);
    }

}

void PrivacyWidget::onMintSelected(bool isMint)
{
    QString btnText;
    if (isMint) {
        btnText = tr("Mint zPIV");
        ui->btnCoinControl->setVisible(true);
        ui->labelSubtitleAmount->setText(tr("Enter amount of PIV to mint into zPIV"));
    } else {
        btnText = tr("Convert back to PIV");
        ui->btnCoinControl->setVisible(false);
        ui->labelSubtitleAmount->setText(tr("Enter amount of zPIV to convert back into PIV"));
    }
    ui->pushButtonSave->setText(btnText);
}

void PrivacyWidget::updateDisplayUnit()
{
    if (walletModel && walletModel->getOptionsModel()) {
        nDisplayUnit = walletModel->getOptionsModel()->getDisplayUnit();

        txHolder->setDisplayUnit(nDisplayUnit);
        ui->listView->update();
    }
}

void PrivacyWidget::showList()
{
    ui->emptyContainer->setVisible(false);
    ui->listView->setVisible(true);
}

void PrivacyWidget::onTotalZpivClicked()
{
    bool isVisible = ui->layoutDenom->isVisible();
    if (!isVisible) {
        ui->layoutDenom->setVisible(true);
        ui->btnTotalzPIV->setRightIconClass("btn-dropdown", true);
    } else {
        ui->layoutDenom->setVisible(false);
        ui->btnTotalzPIV->setRightIconClass("ic-arrow", true);
    }
}

void PrivacyWidget::onSendClicked()
{
    if (!walletModel || !walletModel->getOptionsModel())
        return;

    // Only convert enabled.
    bool isConvert = true;// ui->pushLeft->isChecked();

    WalletModel::UnlockContext ctx(walletModel->requestUnlock());
    if (!ctx.isValid()) {
        inform(tr("You need to unlock the wallet to be able to %1 zPIV").arg(isConvert ? tr("convert") : tr("mint")));
        return;
    }

    bool isValid = true;
    CAmount value = GUIUtil::parseValue(
            ui->lineEditAmount->text(),
            walletModel->getOptionsModel()->getDisplayUnit(),
            &isValid
    );

    if (!isValid || value <= 0) {
        setCssEditLine(ui->lineEditAmount, false, true);
        inform(tr("Invalid value"));
        return;
    }

    setCssEditLine(ui->lineEditAmount, true, true);
    if (isConvert) {
        spend(value);
    } else {
        mint(value);
    }
}

void PrivacyWidget::mint(CAmount value)
{
    std::string strError;
    if (!walletModel->mintCoins(value, CoinControlDialog::coinControl, strError)) {
        inform(tr(strError.data()));
    } else {
        // Mint succeed
        inform(tr("zPIV minted successfully"));
        // clear
        ui->lineEditAmount->clear();
    }
}

void PrivacyWidget::spend(CAmount value)
{
    CZerocoinSpendReceipt receipt;
    std::vector<CZerocoinMint> selectedMints;

    if (!walletModel->convertBackZpiv(
            value,
            selectedMints,
            receipt
    )) {
        inform(receipt.GetStatusMessage().data());
    } else {
        // Spend succeed
        inform(tr("zPIV converted back to PIV"));
        // clear
        ui->lineEditAmount->clear();
    }
}


void PrivacyWidget::onCoinControlClicked()
{
    if (ui->pushRight->isChecked()) {
        if (walletModel->getBalance() > 0) {
            if (!coinControlDialog) {
                coinControlDialog = new CoinControlDialog();
                coinControlDialog->setModel(walletModel);
            } else {
                coinControlDialog->refreshDialog();
            }
            coinControlDialog->exec();
            ui->btnCoinControl->setActive(CoinControlDialog::coinControl->HasSelected());
        } else {
            inform(tr("You don't have any PIV to select."));
        }
    }
}

void PrivacyWidget::onRescanMintsClicked()
{
    if (ask(tr("Rescan Mints"),
        tr("Your zerocoin mints are going to be scanned from the blockchain from scratch"))
    ) {
        std::string strResetMintResult = walletModel->resetMintZerocoin();
        inform(QString::fromStdString(strResetMintResult));
    }
}

void PrivacyWidget::onResetZeroClicked()
{
    if (ask(tr("Reset Spent zPIV"),
        tr("Your zerocoin spends are going to be scanned from the blockchain from scratch"))
    ) {
        std::string strResetMintResult = walletModel->resetSpentZerocoin();
        inform(QString::fromStdString(strResetMintResult));
    }
}

void PrivacyWidget::updateDenomsSupply()
{
 
}

void PrivacyWidget::changeTheme(bool isLightTheme, QString& theme)
{
    static_cast<TxViewHolder*>(this->delegate->getRowFactory())->isLightTheme = isLightTheme;
    ui->listView->update();
}

PrivacyWidget::~PrivacyWidget()
{
    delete ui;
}
