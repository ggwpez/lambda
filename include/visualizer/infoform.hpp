#ifndef INFOFORM_HPP
#define INFOFORM_HPP

#include <interpreter.hpp>

#include <QWidget>
#include <array>
#include <qcustomplot.hpp>

namespace Ui {
class InfoForm;
}

class InfoForm : public QWidget
{
    Q_OBJECT

public:
    explicit InfoForm(QWidget *parent = 0);
    void add_info(inter_info_t& info, bool redraw);
    void add_infos(std::vector<inter_info_t> infos);
    unsigned get_steps() const;
    void update_statistics(const inter_info_t &info, bool redraw);
    void clear();
    ~InfoForm();

private:
    Ui::InfoForm *ui;
    std::size_t steps;
    inter_info_t sum;
    std::array<QCustomPlot*, 5> widgets;
};

#endif // INFOFORM_HPP
