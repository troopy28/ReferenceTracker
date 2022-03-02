#include "VideoPlayer.h"
#include "ui_VideoPlayer.h"

VideoPlayer::VideoPlayer(QWidget* parent) :
	QWidget(parent),
	ui(new Ui::VideoPlayer),
	m_pixmap()
{
	ui->setupUi(this);
	ui->graphicsView->setScene(new QGraphicsScene(this));
	ui->graphicsView->scene()->addItem(&m_pixmap);
}

VideoPlayer::~VideoPlayer()
{
	delete ui;
}