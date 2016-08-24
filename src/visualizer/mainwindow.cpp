#include "mainwindow.hpp"
#include "ui_mainwindow.h"
#include "io.h"

#include <QPainter>
#include <QVector2D>
#include <sstream>
#include <iostream>
#include <QWidget>
#include <array>
#include <cmath>


QColor const    clr_var(QColor(0x80, 0x00, 0xff)),
                clr_app(QColor(0x00, 0xbf, 0xff)),
                clr_abs(QColor(0xff, 0x91, 0x00));

float const     s_var = 2,
                s_app = 1,
                s_abs = 1;

float const     l = 3;
double const    scaleFactor = 1.15;

QPointF const   d_r(sqrt(2), sqrt(2)),
                d_l(-sqrt(2), sqrt(2));

QFont           legend_font("Ubuntu Mono"),
                var_font("Numbus Mono L", 6);

char const* scope_path = "../../../../scope.lam";

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), info(new InfoForm()), ui(new Ui::MainWindow), tree()
{
    ui->setupUi(this);
    scene = new QGraphicsScene(this);
    ui->graphicsView->setScene(scene);
    if (this->ui->chb_aa->isChecked())
        ui->graphicsView->setRenderHints(QPainter::Antialiasing | QPainter::HighQualityAntialiasing);

    info->show();

    std::wstringstream ss;
    if (! io::read_file(scope_path, ss))
    {
        this->scope = ss.str() + std::wstring(L"\n");
        this->ui->checkBox->setCheckable(true);
    }
    else
        std::wcout << L"< File not found: " << scope_path << " >" << std::endl;

    on_chb_aa_toggled(true);
}

void MainWindow::toggle_name_visibility()
{
    names_vis ^= 1;
    for (auto* t : names)
        t->setVisible(names_vis);

    for (auto* r : rects)
        r->setVisible(! names_vis);
}

void MainWindow::showEvent(QShowEvent *e)
{
    QWidget::showEvent(e);
}

void MainWindow::draw_legend(QPointF const& start)
{
    QGraphicsTextItem* var = scene->addText("VAR", legend_font);
    QPointF nstart = QPointF(start.x() -var->boundingRect().size().width(), start.y() -var->boundingRect().size().height());
    var->setPos(nstart);
    var->setDefaultTextColor(clr_var);

    QGraphicsTextItem* abs = scene->addText("ABS", legend_font);
    abs->setPos(nstart.x(), nstart.y() -11);
    abs->setDefaultTextColor(clr_abs);

    QGraphicsTextItem* app = scene->addText("APP", legend_font);
    app->setPos(nstart.x(), nstart.y() -22);
    app->setDefaultTextColor(clr_app);
}

void MainWindow::paintEvent(QPaintEvent* e Q_DECL_UNUSED)
{
    if (tree.get())
    {
        rects.clear();
        names.clear();
        scene->clear();
        //ast_traits::free(tree);
        QPointF const start = QPointF(ui->graphicsView->size().width() /2, ui->graphicsView->size().height() /2);

        scene->addEllipse(start.x() -1.5, start.y() -1.5, 3, 3, QPen(Qt::red), QBrush(Qt::red, Qt::SolidPattern));
        draw_ast(start, tree, 0);
        draw_legend(start);
    }
}

void MainWindow::draw_ast(QPointF start, ast_node_t const& t, int n)
{
    if (t->type == astt::VAR)
    {
        QGraphicsRectItem* rect = scene->addRect(start.x() -s_var /2, start.y() -s_var /2, s_var, s_var, clr_var, QBrush(clr_var, Qt::SolidPattern));
        rect->setVisible(! names_vis);
        rects.push_back(rect);

        if (ui->chb_shadow->checkState() == Qt::Checked)
        {
            QGraphicsTextItem* text = scene->addText(QString::fromWCharArray(t->var.c_str()), var_font);
            QPointF ns = QPointF(start.x() -text->boundingRect().width() /2, start.y() -text->boundingRect().height() /2);

            text->setPos(ns);
            text->setDefaultTextColor(clr_var);
            text->setVisible(names_vis);
            names.push_back(text);
        }
    }
    else
    {
        QPointF left  = start +QPointF(d_l.x() *t->c1->get_width_r(), d_l.y()) *l,
                right = start +QPointF(d_r.x() *t->c2->get_width_l(), d_r.y()) *l;

        scene->addLine(start.x(), start.y(), left.x(), left.y(), (t->type == astt::ABS) ? clr_abs : clr_app);
        scene->addLine(start.x(), start.y(), right.x(), right.y(), (t->type == astt::ABS) ? clr_abs : clr_app);

        draw_ast(left , t->c1, n +1);
        draw_ast(right, t->c2, n +1);
    }
}

int const process_events = 1000;

void MainWindow::eval(bool init_eval, unsigned steps)
{
    //if (info->get_steps() +steps > (unsigned)ui->sb_max_step->value())
      //  steps = std::min(ui->sb_max_step->value() -info->get_steps(), steps);

    if (init_eval)
    {
        QString const& tmp = this->ui->lineEdit->text();

        lexer lex = lexer((this->ui->checkBox->isChecked()) ? this->scope +tmp.toStdWString() : tmp.toStdWString());
        std::vector<tok>* toks = lex.lex();

        parser par = parser(*toks);
        ast_node_t tree = par.parse();

        delete toks;
        this->tree = tree;
        info->clear();
    }
    else if (steps)
    {
        int32_t const redraw_d = this->ui->sb_redraw->value();
        inter_ops_t ops = (inter_ops_t)((int)(ui->chb_eager->isChecked() ? inter_ops_t::NONE : inter_ops_t::LAZY)
                                      | (int)(ui->chb_auto_retry->isChecked() ? inter_ops_t::AUTO_REP_ON_OV : inter_ops_t::NONE));

        interpreter inter(ops);
        bool c;
        uint64_t i = 0;
        ast_node_t old = tree;
        inter_info_t infos;

        while (steps--)
        {
            tree = inter.single_step(old, c, &infos);

            if (c)
                info->add_info(infos, !redraw_d || !(i % redraw_d));
            else
                break;

            if (! (++i % process_events))
                QCoreApplication::processEvents();

            old = tree;
        }
    }
}

void MainWindow::keyPressEvent(QKeyEvent* event)
{
    QWidget::keyPressEvent(event);

    if (event->key() == Qt::Key_N)
        toggle_name_visibility();
    else if (event->key() == Qt::Key_Plus && tree.get())
    {
        eval(false);
        this->repaint();
    }
    else if (event->key() == Qt::Key_P)
    {
        eval(false, 5);
        this->repaint();
    }
    else if (event->key() == Qt::Key_E)
    {
        eval(false, (unsigned)-1);
        this->repaint();
    }
    else if (event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter)
    {
        eval(true);
        this->repaint();
    }
}

void MainWindow::mousePressEvent(QMouseEvent* event)
{
    QWidget::mousePressEvent(event);

    ui->graphicsView->setTransformationAnchor(QGraphicsView::AnchorUnderMouse);

    if (event->button() == Qt::MouseButton::LeftButton)
        ui->graphicsView->scale(scaleFactor, scaleFactor);
    else if (event->button() == Qt::MouseButton::RightButton)
        ui->graphicsView->scale(1.f /scaleFactor, 1.f /scaleFactor);
}

void MainWindow::on_chb_aa_toggled(bool checked)
{
    ui->graphicsView->setRenderHint(QPainter::Antialiasing,             checked);
    ui->graphicsView->setRenderHint(QPainter::HighQualityAntialiasing,  checked);
    ui->graphicsView->setRenderHint(QPainter::TextAntialiasing,         checked);

    this->repaint();
}

void MainWindow::on_chb_shadow_toggled(bool checked Q_DECL_UNUSED)
{
    this->repaint();
}

void MainWindow::closeEvent(QCloseEvent* r)
{
    QWidget::closeEvent(r);
    info->close();
}

MainWindow::~MainWindow()
{
    delete scene;
    ast_traits::free(tree);
    delete info;
    delete ui;
}
