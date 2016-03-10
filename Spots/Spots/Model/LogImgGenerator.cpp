#include "LogImgGenerator.h"


LogImgGenerator::LogImgGenerator()
{
	InitDrawingBoard();
}


LogImgGenerator::~LogImgGenerator()
{
}


void LogImgGenerator::InitDrawingBoard()
{
	BoardHeight = ItemHeight*ItemRows + ItemRowSpan * 2 + (ItemRows - 1)*ItemRowSpan;
	BoardWidth = ItemCount*ItemWidth + ItemColSpan*(ItemCount + 1);

	DrawingBoard.release();
	DrawingBoard = cv::Mat(BoardHeight, BoardWidth, CV_8UC3, cv::Scalar(255, 0, 0));
}

void LogImgGenerator::AddItem(cv::Mat item,string text)
{
	int itemRowHeight = ItemRowSpan + ItemHeight;
	for (int i = ItemRows - 1; i >= 0; i--)
	{
		cv::Mat itemRowROI = DrawingBoard(cv::Rect(0, BoardHeight - 1 - itemRowHeight*(ItemRows - i), BoardWidth, itemRowHeight));
		cv::Mat remainROIImg = itemRowROI(cv::Rect(0, 0, itemRowROI.cols - ItemColSpan - ItemWidth, itemRowHeight)).clone();
		cv::Mat remainROI = itemRowROI(cv::Rect(ItemColSpan + ItemWidth, 0, DrawingBoard.cols - ItemColSpan - ItemWidth, itemRowHeight));
		remainROIImg.copyTo(remainROI);

		cv::Mat addItemROI = itemRowROI(cv::Rect(ItemColSpan, 0, ItemWidth, ItemHeight));
		if (i != 0)
		{
			//把上一行最后一个放到本行第一个
			cv::Mat replaceItem = DrawingBoard(cv::Rect(BoardWidth - 1 - ItemWidth - ItemColSpan, BoardHeight - 1 - itemRowHeight*(ItemRows - i + 1), ItemWidth, ItemHeight));
			replaceItem.copyTo(addItemROI);
		}
		else
		{
			//把新图加入第一行第一个
			cv::resize(item, addItemROI, cv::Size(ItemWidth, ItemHeight));
			cv::putText(addItemROI, text, cv::Point(0, ItemHeight - 5), 1, 1, cv::Scalar(0, 0, 255));
		}
	}
}