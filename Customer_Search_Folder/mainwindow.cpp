        /****************************************************************************
**
** Copyright (C) 2016 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** BSD License Usage
** Alternatively, you may use this file under the terms of the BSD license
** as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of The Qt Company Ltd nor the names of its
**     contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QtWidgets>

#include "mainwindow.h"

#include <iostream>



#include <sstream>

#include <string>

#include <dlfcn.h>



typedef std::string (*Send_Request)(std::string);
typedef void (*Visualize)(std::string);

//! [17]
enum { absoluteFileNameRole = Qt::UserRole + 1 };
//! [17]

//! [18]
static inline QString fileNameOfItem(const QTableWidgetItem *item)
{
    return item->data(absoluteFileNameRole).toString();
}
//! [18]

//! [14]
static inline void openFile(const QString &fileName)
{
    QDesktopServices::openUrl(QUrl::fromLocalFile(fileName));
}
//! [14]

//! [0]
Window::Window(QWidget *parent)
    : QWidget(parent)
{
    setWindowTitle(tr("Customer Search"));
    findButton = new QPushButton(tr("&Search"), this);
    connect(findButton, &QAbstractButton::clicked, this, &Window::find);

    storeCodeComboBox = createComboBox(tr(""));
    connect(storeCodeComboBox->lineEdit(), &QLineEdit::returnPressed,
            this, &Window::animateFindClick);
    numberComboBox = createComboBox(tr(""));
    connect(numberComboBox->lineEdit(), &QLineEdit::returnPressed,
            this, &Window::animateFindClick);
    cardnumberComboBox = createComboBox(tr(""));
    connect(cardnumberComboBox->lineEdit(), &QLineEdit::returnPressed,
            this, &Window::animateFindClick);
    hostComboBox = createComboBox(tr("https://localhost/customers"));
    connect(hostComboBox->lineEdit(), &QLineEdit::returnPressed,
            this, &Window::animateFindClick);

    filesFoundLabel = new QLabel;

    createFilesTable();

    QGridLayout *mainLayout = new QGridLayout(this);
    mainLayout->addWidget(new QLabel(tr("storeCode:")), 0, 0);
    mainLayout->addWidget(storeCodeComboBox, 0, 1, 1, 2);
    mainLayout->addWidget(new QLabel(tr("number:")), 1, 0);
    mainLayout->addWidget(numberComboBox, 1, 1, 1, 2);
    mainLayout->addWidget(new QLabel(tr("cardNumber:")), 2, 0);
    mainLayout->addWidget(cardnumberComboBox, 2, 1, 1, 2);
    mainLayout->addWidget(new QLabel(tr("Host:")), 3, 0);
    mainLayout->addWidget(hostComboBox, 3, 2);
    mainLayout->addWidget(filesTable, 4, 0, 1, 3);
    mainLayout->addWidget(filesFoundLabel, 5, 0, 1, 2);
    mainLayout->addWidget(findButton, 5, 2);
//! [0]

//! [1]
    connect(new QShortcut(QKeySequence::Quit, this), &QShortcut::activated,
        qApp, &QApplication::quit);
//! [1]
}

static void updateComboBox(QComboBox *comboBox)
{
    if (comboBox->findText(comboBox->currentText()) == -1)
        comboBox->addItem(comboBox->currentText());
}

//! [3]
void Window::find()
{
    void* handle = dlopen("./libSearcher.so", RTLD_LAZY); // creates dynamic linker to the shared library
    if (!handle) { // tests if the shared library could be found
        std::cout << "Could not open the library" << std::endl;
        return;
    }
    std:: string request = create_request();
    std::cout << "Request: " << request << std::endl;
    Send_Request get = reinterpret_cast<Send_Request>(dlsym(handle, "send_request"));
    Visualize vizualize = reinterpret_cast<Visualize>(dlsym(handle, "visualize_response"));
    std::string Response = get(request);
    vizualize(Response);
    dlclose(handle);
    json response =   json::parse(Response);
    response_to_table(response);
}
//! [4]

void Window::animateFindClick()
{
    findButton->animateClick();
}

QComboBox *Window::createComboBox(const QString &text)
{
    QComboBox *comboBox = new QComboBox;
    comboBox->setEditable(true);
    comboBox->addItem(text);
    comboBox->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    return comboBox;
}
//! [10]

//! [11]
void Window::createFilesTable()
{
    filesTable = new QTableWidget(0, 7);
    filesTable->setSelectionBehavior(QAbstractItemView::SelectRows);

    QStringList labels;
    labels << tr("Lastname") << tr("Firstname") << tr("Salutation") <<  tr("Number") << tr("UUID") << tr("City") << tr("Street") ;
    filesTable->setHorizontalHeaderLabels(labels);
    //filesTable->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
    filesTable->verticalHeader()->hide();
    filesTable->setShowGrid(true);
//! [15]
    filesTable->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(filesTable, &QTableWidget::customContextMenuRequested,
            this, &Window::contextMenu);
    connect(filesTable, &QTableWidget::cellActivated,
            this, &Window::openFileOfItem);
//! [15]
}
//! [11]

std::string Window::create_request(){
    std::stringstream stream;
    stream << hostComboBox->currentText().toStdString() << "?storeCode=" << storeCodeComboBox->currentText().toStdString() << "&number=" << numberComboBox->currentText().toStdString()
           <<  "&cardNumber=" << cardnumberComboBox->currentText().toStdString() ;
    return stream.str();
}

void Window::response_to_table(json response){
   filesTable->setRowCount(0);
    for(int i = 0; i < (int) response.size();++i){
        filesTable->insertRow(i);
        if(response[(unsigned long)i].find("firstName") != response[(unsigned long)i].end()){
            Items.push_back(new QTableWidgetItem(QString::fromStdString(response[(unsigned long)i]["firstName"])));
            filesTable->setItem(i, 0,Items[Items.size()-1]);
        }
        if(response[(unsigned long)i].find("lastName") != response[(unsigned long)i].end()){
            Items.push_back(new QTableWidgetItem(QString::fromStdString(response[(unsigned long)i]["lastName"])));
            filesTable->setItem(i, 1,Items[Items.size()-1]);
        }
        if(response[(unsigned long)i].find("salutation") != response[(unsigned long)i].end()){
            Items.push_back(new QTableWidgetItem(QString::fromStdString(response[(unsigned long)i]["salutation"])));
            filesTable->setItem(i, 2,Items[Items.size()-1]);
        }
        if(response[(unsigned long)i].find("number") != response[(unsigned long)i].end()){
            Items.push_back(new QTableWidgetItem(QString::fromStdString(response[(unsigned long)i]["number"])));
            filesTable->setItem(i, 3,Items[Items.size()-1]);
        }
        if(response[(unsigned long)i].find("uuid") != response[(unsigned long)i].end()){
             Items.push_back(new QTableWidgetItem(QString::fromStdString(response[(unsigned long)i]["uuid"])));
             filesTable->setItem(i, 4,Items[Items.size()-1]);
        }
        if(response[(unsigned long)i].find("communicationAddress") != response[(unsigned long)i].end()){
             if(response[(unsigned long)i]["communicationAddress"].find("city") != response[(unsigned long)i]["communicationAddress"].end()){
                Items.push_back(new QTableWidgetItem(QString::fromStdString(response[(unsigned long)i]["communicationAddress"]["city"])));
                filesTable->setItem(i, 5,Items[Items.size()-1]);
             }
             if(response[(unsigned long)i]["communicationAddress"].find("street") != response[(unsigned long)i]["communicationAddress"].end()){
                Items.push_back(new QTableWidgetItem(QString::fromStdString(response[(unsigned long)i]["communicationAddress"]["street"])));
                filesTable->setItem(i, 6,Items[Items.size()-1]);
             }
        }
    }
}

//! [12]

void Window::openFileOfItem(int row, int /* column */)
{
    const QTableWidgetItem *item = filesTable->item(row, 0);
    openFile(fileNameOfItem(item));
}

//! [12]

//! [16]
void Window::contextMenu(const QPoint &pos)
{
    const QTableWidgetItem *item = filesTable->itemAt(pos);
    if (!item)
        return;
    QMenu menu;
#ifndef QT_NO_CLIPBOARD
    QAction *copyAction = menu.addAction("Copy Name");
#endif
    QAction *openAction = menu.addAction("Open");
    QAction *action = menu.exec(filesTable->mapToGlobal(pos));
    if (!action)
        return;
    const QString fileName = fileNameOfItem(item);
    if (action == openAction)
        openFile(fileName);
#ifndef QT_NO_CLIPBOARD
    else if (action == copyAction)
        QGuiApplication::clipboard()->setText(QDir::toNativeSeparators(fileName));
#endif
}
//! [16]
