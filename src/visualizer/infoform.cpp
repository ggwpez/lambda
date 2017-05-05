#include <infoform.hpp>
#include <ui_infoform.h>

InfoForm::InfoForm(QWidget *parent)
	: QWidget(parent), ui(new Ui::InfoForm), steps(0)
{
	ui->setupUi(this);

	widgets = {{ ui->plot_alpha, ui->plot_beta, ui->plot_eta, ui->plot_tau, ui->plot_node }};

	this->ui->plot_node->yAxis->setLabel("nodes");
	this->ui->plot_alpha->yAxis->setLabel("alphas");
	this->ui->plot_beta->yAxis->setLabel("betas");
	this->ui->plot_eta->yAxis->setLabel("etas");
	this->ui->plot_tau->yAxis->setLabel("taus");

	for (QCustomPlot* w : widgets)
	{
		w->addGraph();
		w->xAxis->setLabel("steps");
		w->replot();
	}
}

unsigned InfoForm::get_steps() const
{
	return steps;
}

void InfoForm::add_info(inter_info_t& info, bool redraw)
{
	for (size_t i = 0; i < widgets.size(); i++)
	{
		QCPGraph* g = widgets[i]->graph(0);
		QCPData map = QCPData(steps, reinterpret_cast<uint64_t*>(&info)[i]);

		g->addData(map);

		if (redraw)
			widgets[i]->rescaleAxes(),
			widgets[i]->replot();
	}

	update_statistics(info, redraw);
	steps++;
}

void InfoForm::update_statistics(inter_info_t const& info, bool redraw)
{
	this->sum += info;

	this->ui->lcd_steps->display(QString::number(steps));
	this->ui->lcd_nodes->display(QString::number(sum.nodes));
	this->ui->lcd_alphas->display(QString::number(sum.alphas));
	this->ui->lcd_betas->display(QString::number(sum.betas));
	this->ui->lcd_etas->display(QString::number(sum.etas));
	this->ui->lcd_taus->display(QString::number(sum.taus));

	this->ui->lcd_step->display(QString::number(1));
	this->ui->lcd_node->display(QString::number(info.nodes));
	this->ui->lcd_alpha->display(QString::number(info.alphas));
	this->ui->lcd_beta->display(QString::number(info.betas));
	this->ui->lcd_eta->display(QString::number(info.etas));
	this->ui->lcd_tau->display(QString::number(info.taus));

	if (redraw)
		this->ui->lcd_steps->repaint();
}

void InfoForm::clear()
{
	steps = 0;
	this->sum.clear();

	for (QCustomPlot* w : widgets)
	{
		w->graph(0)->clearData();
		w->replot();
	}
}

InfoForm::~InfoForm()
{
	delete ui;
}
