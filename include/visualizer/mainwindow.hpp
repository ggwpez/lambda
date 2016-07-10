#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP

#include "ast.hpp"
#include "lexer.hpp"
#include "interpreter.hpp"
#include "parser.hpp"
#include "infoform.hpp"

#include <QMainWindow>
#include <QGraphicsScene>
#include <QShowEvent>
#include <QGraphicsItem>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = 0);
    void paintEvent(QPaintEvent* e);
    ~MainWindow();

private slots:
    void eval(bool init_eval, unsigned steps = 1);
    void draw_ast(QPointF start, ast_node_t const& t, int n);
    void closeEvent(QCloseEvent* r);
    virtual void mousePressEvent(QMouseEvent* event);
    virtual void keyPressEvent(QKeyEvent* event);
    virtual void showEvent(QShowEvent *e);

    void on_chb_aa_toggled(bool checked);

    void on_chb_shadow_toggled(bool checked Q_DECL_UNUSED);

private:
    void draw_legend(const QPointF &start);
    void toggle_name_visibility();

    std::vector<QGraphicsRectItem*> rects;
    std::vector<QGraphicsTextItem*> names;
    bool names_vis = false;

    InfoForm* info;
    Ui::MainWindow* ui;
    ast_node_t tree;
    std::wstring scope;
    QGraphicsScene *scene;
};

#endif // MAINWINDOW_HPP
