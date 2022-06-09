#include <QtWidgets>
#if defined(QT_PRINTSUPPORT_LIB)
#include <QtPrintSupport/qtprintsupportglobal.h>
#if QT_CONFIG(printdialog)
#include <QPrinter>
#include <QPrintDialog>
#endif
#endif

#include "scribblearea.h"

scribblearea::scribblearea(QWidget *parent)
    : QWidget(parent)
{
    setAttribute(Qt::WA_StaticContents);

    modified = false;
    scribbling = false;
    myPenWidth = 1;
    myPenColor = Qt::blue;
}


bool scribblearea::openImage(const QString &fileName)
{
    QImage loadedImage;

    if (!loadedImage.load(fileName))
        return false;

    QSize newSize = loadedImage.size().expandedTo(size());
    resizeImage(&loadedImage, newSize);
    image = loadedImage;
    modified = false;
    update();
    return true;
}


bool scribblearea::saveImage(const QString &fileName, const char *fileFormat)
{
    QImage visibleImage = image;
    resizeImage(&visibleImage, size());

    if (visibleImage.save(fileName, fileFormat)) {
        modified = false;
        return true;
    } else {
        return false;
    }
}


void scribblearea::setPenColor(const QColor &newColor)
{
    myPenColor = newColor;
}


void scribblearea::setPenWidth(int newWidth)
{
    myPenWidth = newWidth;
}


void scribblearea::clearImage()
{
    image.fill(qRgb(255, 255, 255));
    modified = true;
    update();
}


void scribblearea::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        lastPoint = event->pos();
        scribbling = true;
    }
}


void scribblearea::mouseMoveEvent(QMouseEvent *event)
{
    if ((event->buttons() & Qt::LeftButton) && scribbling)
        drawLineTo(event->pos());
}


void scribblearea::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton && scribbling) {
        drawLineTo(event->pos());
        scribbling = false;
    }
}


void scribblearea::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);

    QRect dirtyRect = event->rect();
    painter.drawImage(dirtyRect, image, dirtyRect);
}


void scribblearea::resizeEvent(QResizeEvent *event)
{
    if (width() > image.width() || height() > image.height()) {
        int newWidth = qMax(width() + 128, image.width());
        int newHeight = qMax(height() + 128, image.height());
        resizeImage(&image, QSize(newWidth, newHeight));
        update();
    }
    QWidget::resizeEvent(event);
}

void scribblearea::drawLineTo(const QPoint &endPoint)
{

    QPainter painter(&image);
    painter.setPen(QPen(myPenColor, myPenWidth, Qt::SolidLine, Qt::RoundCap,
                        Qt::RoundJoin));

    painter.drawLine(lastPoint, endPoint);
    modified = true;

    int rad = (myPenWidth / 2) + 2;

    update(QRect(lastPoint, endPoint).normalized()
                                     .adjusted(-rad, -rad, +rad, +rad));

    lastPoint = endPoint;
}


void scribblearea::resizeImage(QImage *image, const QSize &newSize)
{
    if (image->size() == newSize)
        return;

    QImage newImage(newSize, QImage::Format_RGB32);
    newImage.fill(qRgb(255, 255, 255));

    QPainter painter(&newImage);
    painter.drawImage(QPoint(0, 0), *image);
    *image = newImage;
}


void scribblearea::print()
{

    #if QT_CONFIG(printdialog)

        QPrinter printer(QPrinter::HighResolution);

        QPrintDialog printDialog(&printer, this);
        if (printDialog.exec() == QDialog::Accepted) {
            QPainter painter(&printer);
            QRect rect = painter.viewport();
            QSize size = image.size();
            size.scale(rect.size(), Qt::KeepAspectRatio);
            painter.setViewport(rect.x(), rect.y(), size.width(), size.height());
            painter.setWindow(image.rect());
            painter.drawImage(0, 0, image);
        }
    #endif

}
