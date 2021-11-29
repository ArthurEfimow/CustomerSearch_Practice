#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QWidget>
#include <QDir>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

QT_BEGIN_NAMESPACE
class QComboBox;
class QLabel;
class QPushButton;
class QTableWidget;
class QTableWidgetItem;
QT_END_NAMESPACE

class Window : public QWidget
{
    Q_OBJECT

public:
    explicit Window(QWidget *parent = 0);

private slots:
    void find();
    void animateFindClick();
    void openFileOfItem(int row, int column);
    void contextMenu(const QPoint &pos);

private:
    QComboBox *createComboBox(const QString &text = QString());
    void createFilesTable();
    std::string create_request();
    void response_to_table(json);
    QComboBox *storeCodeComboBox;
    QComboBox *numberComboBox;
    QComboBox *cardnumberComboBox;
    QComboBox *hostComboBox;
    QLabel *filesFoundLabel;
    QPushButton *findButton;
    QTableWidget *filesTable;

    QDir currentDir;

    std::vector<QTableWidgetItem*> Items;
};


#endif // MAINWINDOW_H
