#include "dspdfviewer.h"
#include "renderutils.h"
#include "renderingidentifier.h"

#include <QtGui/QLabel>
#include <QtGui/QMenu>
#include <QtGui/QMenuBar>
#include <QtGui/QAction>
#include <QApplication>
#include <QDesktopWidget>
#include <qlayout.h>

#include <QDebug>
#include <stdexcept>

DSPDFViewer::DSPDFViewer(QString filename): pdfDocument(
  Poppler::Document::load(filename)
),
 audienceWindow(0, PagePart::LeftHalf),
 secondaryWindow(1, PagePart::RightHalf),
 m_pagenumber(0),
 renderFactory(filename)
{
  audienceWindow.setViewer(this);
  secondaryWindow.setViewer(this);
  audienceWindow.showLoadingScreen(0);
  secondaryWindow.showLoadingScreen(0);
  secondaryWindow.showInformationLine();
  // secondaryWindow.hideInformationLine();
  if ( ! pdfDocument  || pdfDocument->isLocked() )
  {
    /// FIXME: Error message
    throw std::runtime_error("I was not able to open the PDF document. Sorry.");
  }
  setHighQuality(true);
  
  connect( &renderFactory, SIGNAL(pageRendered(QSharedPointer<RenderedPage>)), &audienceWindow, SLOT(renderedPageIncoming(QSharedPointer<RenderedPage>)));
  connect( &renderFactory, SIGNAL(pageRendered(QSharedPointer<RenderedPage>)), &secondaryWindow, SLOT(renderedPageIncoming(QSharedPointer<RenderedPage>)));
  
  connect( &renderFactory, SIGNAL(thumbnailRendered(QSharedPointer<RenderedPage>)), &audienceWindow, SLOT(renderedThumbnailIncoming(QSharedPointer<RenderedPage>)));
  connect( &renderFactory, SIGNAL(thumbnailRendered(QSharedPointer<RenderedPage>)), &secondaryWindow, SLOT(renderedThumbnailIncoming(QSharedPointer<RenderedPage>)));
  
  renderPage();
}



DSPDFViewer::~DSPDFViewer()
{}

void DSPDFViewer::goBackward()
{
  gotoPage(pageNumber()-1);
}

void DSPDFViewer::goForward()
{
  gotoPage(pageNumber()+1);
}

unsigned int DSPDFViewer::pageNumber()
{
  return m_pagenumber;
}

QImage DSPDFViewer::renderForTarget(QSharedPointer< Poppler::Page > page, QSize targetSize, bool onlyHalf, bool rightHalf)
{
  PagePart part( 
    onlyHalf?
      ( rightHalf? PagePart::RightHalf : PagePart::LeftHalf )
      : PagePart::FullPage );
  return RenderUtils::renderPagePart(page, targetSize, part);

}


void DSPDFViewer::renderPage()
{
#if 0
for( unsigned int i=std::max(3u, m_pagenumber)-3; i<m_pagenumber+6; i++)
  {
    if ( ! secondaryWindow.hasThumbnailForPage(i) )
    {
      QSharedPointer<Poppler::Page> page( pdfDocument->page(i) );
      if ( page ) {
	QImage thumbnail = renderForTarget(page, thumbnailSize, false);
	secondaryWindow.addThumbnail(i, thumbnail);
      }
    }
  }
  secondaryWindow.renderThumbnails(m_pagenumber);
#endif /* 0 */

  qDebug() << "Requesting rendering of page " << m_pagenumber;
  audienceWindow.showLoadingScreen(m_pagenumber);
  secondaryWindow.showLoadingScreen(m_pagenumber);
  theFactory()->requestThumbnailRendering(m_pagenumber);
  theFactory()->requestPageRendering(m_pagenumber, audienceWindow.getTargetImageSize(), PagePart::LeftHalf);
  theFactory()->requestPageRendering(m_pagenumber, secondaryWindow.getTargetImageSize(), PagePart::RightHalf);
  
  /** Pre-Render next 10 pages **/
  for ( int i=m_pagenumber; i<m_pagenumber+10 && i < pdfDocument->numPages() ; i++) {
    theFactory()->requestThumbnailRendering(i);
    theFactory()->requestPageRendering(i, audienceWindow.getTargetImageSize(), PagePart::LeftHalf);
    theFactory()->requestPageRendering(i, secondaryWindow.getTargetImageSize(), PagePart::RightHalf);
  }
  
  /** Request previous 3 pages **/
  
  for ( int i= std::max(m_pagenumber,3u)-3; i<m_pagenumber; i++) {
    theFactory()->requestThumbnailRendering(i);
    theFactory()->requestPageRendering(i, audienceWindow.getTargetImageSize(), PagePart::LeftHalf);
    theFactory()->requestPageRendering(i, secondaryWindow.getTargetImageSize(), PagePart::RightHalf);
  }
  
}


void DSPDFViewer::gotoPage(unsigned int pageNumber)
{
  if ( m_pagenumber != pageNumber 
      && pdfDocument->numPages() > pageNumber )
  {
    m_pagenumber = pageNumber;
    renderPage();
  }
}

void DSPDFViewer::swapScreens()
{
  if ( audienceWindow.getMonitor() == 0 )
  {
    audienceWindow.setMonitor(1);
    secondaryWindow.setMonitor(0);
    renderPage();
  }
  else
  {
    audienceWindow.setMonitor(0);
    secondaryWindow.setMonitor(1);
    renderPage();
  }
}



void DSPDFViewer::exit()
{
  audienceWindow.close();
  secondaryWindow.close();
}

void DSPDFViewer::setHighQuality(bool hq)
{
  pdfDocument->setRenderHint(Poppler::Document::Antialiasing, hq);
  pdfDocument->setRenderHint(Poppler::Document::TextAntialiasing, hq);
  pdfDocument->setRenderHint(Poppler::Document::TextHinting, hq);
}

const QSize DSPDFViewer::thumbnailSize = QSize(200, 100);

PdfRenderFactory* DSPDFViewer::theFactory()
{
  return &renderFactory;
}


#include "dspdfviewer.moc"
