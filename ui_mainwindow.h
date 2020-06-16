/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 5.5.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QFrame>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QProgressBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralWidget;
    QVBoxLayout *verticalLayout;
    QHBoxLayout *horizontalLayout_4;
    QPushButton *bootTriggerButton;
    QComboBox *bootModeSelector;
    QProgressBar *progressBar;
    QFrame *line;
    QVBoxLayout *verticalLayout_2;
    QHBoxLayout *horizontalLayout;
    QLineEdit *filePathEdit;
    QPushButton *searchPath;
    QHBoxLayout *horizontalLayout_2;
    QLabel *label;
    QComboBox *comSelector;
    QPushButton *getStatus;
    QHBoxLayout *horizontalLayout_5;
    QLabel *label_3;
    QComboBox *protocolSelector;
    QHBoxLayout *horizontalLayout_3;
    QLabel *label_2;
    QSpinBox *pageSizeSpin;
    QSpacerItem *verticalSpacer;
    QPushButton *flashButton;
    QMenuBar *menuBar;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QStringLiteral("MainWindow"));
        MainWindow->resize(400, 300);
        centralWidget = new QWidget(MainWindow);
        centralWidget->setObjectName(QStringLiteral("centralWidget"));
        verticalLayout = new QVBoxLayout(centralWidget);
        verticalLayout->setSpacing(6);
        verticalLayout->setContentsMargins(11, 11, 11, 11);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        horizontalLayout_4 = new QHBoxLayout();
        horizontalLayout_4->setSpacing(6);
        horizontalLayout_4->setObjectName(QStringLiteral("horizontalLayout_4"));
        bootTriggerButton = new QPushButton(centralWidget);
        bootTriggerButton->setObjectName(QStringLiteral("bootTriggerButton"));
        bootTriggerButton->setAutoFillBackground(false);
        bootTriggerButton->setStyleSheet(QStringLiteral(""));
        bootTriggerButton->setCheckable(true);
        bootTriggerButton->setChecked(false);
        bootTriggerButton->setFlat(false);

        horizontalLayout_4->addWidget(bootTriggerButton);

        bootModeSelector = new QComboBox(centralWidget);
        bootModeSelector->setObjectName(QStringLiteral("bootModeSelector"));

        horizontalLayout_4->addWidget(bootModeSelector);

        progressBar = new QProgressBar(centralWidget);
        progressBar->setObjectName(QStringLiteral("progressBar"));
        progressBar->setMaximum(100);
        progressBar->setValue(0);
        progressBar->setTextVisible(true);
        progressBar->setOrientation(Qt::Horizontal);
        progressBar->setInvertedAppearance(false);

        horizontalLayout_4->addWidget(progressBar);


        verticalLayout->addLayout(horizontalLayout_4);

        line = new QFrame(centralWidget);
        line->setObjectName(QStringLiteral("line"));
        line->setFrameShape(QFrame::HLine);
        line->setFrameShadow(QFrame::Sunken);

        verticalLayout->addWidget(line);

        verticalLayout_2 = new QVBoxLayout();
        verticalLayout_2->setSpacing(6);
        verticalLayout_2->setObjectName(QStringLiteral("verticalLayout_2"));
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setSpacing(6);
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        filePathEdit = new QLineEdit(centralWidget);
        filePathEdit->setObjectName(QStringLiteral("filePathEdit"));

        horizontalLayout->addWidget(filePathEdit);

        searchPath = new QPushButton(centralWidget);
        searchPath->setObjectName(QStringLiteral("searchPath"));

        horizontalLayout->addWidget(searchPath);


        verticalLayout_2->addLayout(horizontalLayout);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setSpacing(6);
        horizontalLayout_2->setObjectName(QStringLiteral("horizontalLayout_2"));
        label = new QLabel(centralWidget);
        label->setObjectName(QStringLiteral("label"));
        label->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        horizontalLayout_2->addWidget(label);

        comSelector = new QComboBox(centralWidget);
        comSelector->setObjectName(QStringLiteral("comSelector"));
        comSelector->setMaximumSize(QSize(125, 16777215));

        horizontalLayout_2->addWidget(comSelector);

        getStatus = new QPushButton(centralWidget);
        getStatus->setObjectName(QStringLiteral("getStatus"));

        horizontalLayout_2->addWidget(getStatus);


        verticalLayout_2->addLayout(horizontalLayout_2);

        horizontalLayout_5 = new QHBoxLayout();
        horizontalLayout_5->setSpacing(6);
        horizontalLayout_5->setObjectName(QStringLiteral("horizontalLayout_5"));
        label_3 = new QLabel(centralWidget);
        label_3->setObjectName(QStringLiteral("label_3"));
        label_3->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        horizontalLayout_5->addWidget(label_3);

        protocolSelector = new QComboBox(centralWidget);
        protocolSelector->setObjectName(QStringLiteral("protocolSelector"));

        horizontalLayout_5->addWidget(protocolSelector);


        verticalLayout_2->addLayout(horizontalLayout_5);

        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setSpacing(6);
        horizontalLayout_3->setObjectName(QStringLiteral("horizontalLayout_3"));
        label_2 = new QLabel(centralWidget);
        label_2->setObjectName(QStringLiteral("label_2"));
        label_2->setEnabled(true);
        label_2->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        horizontalLayout_3->addWidget(label_2);

        pageSizeSpin = new QSpinBox(centralWidget);
        pageSizeSpin->setObjectName(QStringLiteral("pageSizeSpin"));
        pageSizeSpin->setEnabled(true);
        pageSizeSpin->setMaximum(1024);
        pageSizeSpin->setValue(128);

        horizontalLayout_3->addWidget(pageSizeSpin);


        verticalLayout_2->addLayout(horizontalLayout_3);


        verticalLayout->addLayout(verticalLayout_2);

        verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout->addItem(verticalSpacer);

        flashButton = new QPushButton(centralWidget);
        flashButton->setObjectName(QStringLiteral("flashButton"));

        verticalLayout->addWidget(flashButton);

        MainWindow->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(MainWindow);
        menuBar->setObjectName(QStringLiteral("menuBar"));
        menuBar->setEnabled(false);
        menuBar->setGeometry(QRect(0, 0, 400, 21));
        MainWindow->setMenuBar(menuBar);
        statusBar = new QStatusBar(MainWindow);
        statusBar->setObjectName(QStringLiteral("statusBar"));
        MainWindow->setStatusBar(statusBar);

        retranslateUi(MainWindow);

        bootTriggerButton->setDefault(false);


        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QApplication::translate("MainWindow", "MEMO ECU Flasher", 0));
        bootTriggerButton->setText(QApplication::translate("MainWindow", "Boot mode", 0));
        bootModeSelector->clear();
        bootModeSelector->insertItems(0, QStringList()
         << QApplication::translate("MainWindow", "Normal", 0)
         << QApplication::translate("MainWindow", "Bridge", 0)
        );
        filePathEdit->setPlaceholderText(QApplication::translate("MainWindow", "Path to *.hex flash file", 0));
        searchPath->setText(QApplication::translate("MainWindow", "Search", 0));
        label->setText(QApplication::translate("MainWindow", "Select COM port:", 0));
        getStatus->setText(QApplication::translate("MainWindow", "Get status", 0));
        label_3->setText(QApplication::translate("MainWindow", "Protocol:", 0));
        label_2->setText(QApplication::translate("MainWindow", "Page size:", 0));
        flashButton->setText(QApplication::translate("MainWindow", "Flash!", 0));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
