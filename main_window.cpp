/***************************************************************************
 *   Copyright (C) 2009 by Guy Rutenberg   *
 *   guyrutenberg@gmail.com   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#include <QtGui>

#include "main_window.h"

MainWindow::MainWindow()
    : QMainWindow()
{
	setupUi(this);

	setFixedHeight(sizeHint().height());
}

void MainWindow::on_calculateButton_clicked()
{
	checksumEdit->setText("");

	QFile file(fileEdit->text());
	file.open(QIODevice::ReadOnly);

	const int block_size = (file.size() > 1024*1024) ? 10*1024 : 1024;
	char buffer[block_size];
	int bytes_read;

	int progress_max = file.size()/block_size;
	progress_max = (progress_max > 0) ? progress_max : 1;
	progressBar->setMaximum(progress_max);
	progressBar->reset();

	QCryptographicHash::Algorithm hash_alg = static_cast<QCryptographicHash::Algorithm>(hashSelector->currentIndex());
	QCryptographicHash hash(hash_alg);

	while( (bytes_read = file.read(buffer, block_size))>0) {
		hash.addData(buffer, bytes_read);
		progressBar->setValue(progressBar->value()+1);
	}
	
	// prevents progressBar from stopping on 99%;
	progressBar->setValue(progressBar->maximum());

	checksumEdit->setText(hash.result().toHex());
	file.close();
}

void MainWindow::on_fileEdit_textChanged(const QString &text)
{
	QFileInfo f_info(text);
	if(f_info.isFile()) {
		calculateButton->setEnabled(true);
	} else {
		calculateButton->setEnabled(false);
	}
}

void MainWindow::on_browseButton_clicked()
{
	QString filename = QFileDialog::getOpenFileName(this, tr("Load file..."));
	if(!filename.isEmpty()) {
		fileEdit->setText(filename);
	}
}

void MainWindow::on_copyButton_clicked()
{
	QApplication::clipboard()->setText(checksumEdit->text());
}

void MainWindow::dragEnterEvent(QDragEnterEvent *event)
{
	if (event->mimeData()->hasFormat("text/uri-list"))
		event->acceptProposedAction();
}

void MainWindow::dropEvent(QDropEvent *event)
{
	QList<QUrl> urls = event->mimeData()->urls();
	if (urls.isEmpty())
		return;

	QString fileName = urls.first().toLocalFile();
	fileEdit->setText(fileName);
	calculateButton->click();
}
