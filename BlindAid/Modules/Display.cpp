#include "Display.h"

using namespace std;
using namespace std::chrono;
using namespace cv;

namespace Display
{
  Display::Display(IParameters *params, IData *input, IData *output) : IModule(params, input, output)
  {

  }

  void Display::Process()
  {
    steady_clock::time_point start = steady_clock::now();

    DrawDepthObstacles();
    DrawTrafficLights();
    DrawStopSign();
    DisplayImage();

    steady_clock::time_point end = steady_clock::now();
    duration<double> time_span = duration_cast<duration<double>>(end - start);

    cout << "[DISPLAY] Frame displayed (" << time_span.count() * 1000 << "ms).\n";
  }

  void Display::DrawDepthObstacles()
  {
    _output->GetDepthImage()->convertTo(*_input->GetDepthOverlayImage(), CV_8UC1, 1.f / 8.f, -0.5 / 8.f); // , 255.0 / (5 - 0.5));
    if (_input->GetDepthOverlayImage()->channels() == 1)
      cvtColor(*_input->GetDepthOverlayImage(), *_input->GetDepthOverlayImage(), CV_GRAY2BGR);

    Rect rect;
    for (int j = 0; j < VERT_REGIONS; ++j)
    {
      for (int i = 0; i < HORZ_REGIONS; ++i)
    {
        rect = _input->GetDepthObstacleResults()->GetRegion(j, i);
        rectangle(*_input->GetDepthOverlayImage(), rect, Scalar(0, 0, 255), 2);
        putText(*_input->GetDepthOverlayImage(), to_string(_input->GetDepthObstacleResults()->GetDepth(j, i)), Point(rect.x + (int)(0.5 * rect.width) - 25, rect.y + (int)(0.5 * rect.height)), FONT_HERSHEY_PLAIN, 1.25, Scalar(0, 0, 255), 2);
      }

      (*_input->GetDepthOverlayImage())(cv::Rect(j * 60, 0, 60, 30)).setTo((int)_input->GetDepthObstacleResults()->GetVibration(j)->Get());
      putText(*_input->GetDepthOverlayImage(), to_string((int)_input->GetDepthObstacleResults()->GetVibration(j)->Get()), Point(j * 60 + 7, 22), FONT_HERSHEY_PLAIN, 1.5, (int)_input->GetDepthObstacleResults()->GetVibration(j)->Get() > 127 ? Scalar(0, 0, 0) : Scalar(255, 255, 255), 2);
    }
  }

  void Display::DrawTrafficLights()
  {
    _output->GetColorImage()->copyTo(*_input->GetColorOverlayImage());

    vector<Vision::TrafficLight::Result> result = _input->GetTrafficLightResults()->Get();

    if (result.size() == 1 && result.at(0).GetCenter() == Point(0, 0))
    {
      (*_input->GetColorOverlayImage())(cv::Rect(480, 0, 240, 60)).setTo(Scalar(255, 255, 255));
      putText(*_input->GetColorOverlayImage(), _input->GetTrafficLightResults()->_names[result.at(0).GetColor()], Point(500, 45), FONT_HERSHEY_PLAIN, 3, _input->GetTrafficLightResults()->_colors[result.at(0).GetColor()], 2);

      for (int j = 0; j < 4; ++j)
      {
        (*_input->GetColorOverlayImage())(cv::Rect(j * 120, 0, 120, 60)).setTo(_input->GetTrafficLightResults()->_colors[j] * max(0.25f, result.at(0).GetConfidence((Vision::TrafficLight::Result::Color)j)));
        putText(*_input->GetColorOverlayImage(), to_string(result.at(0).GetConfidence((Vision::TrafficLight::Result::Color)j)).substr(0, 4), Point(j * 120 + 10, 45), FONT_HERSHEY_PLAIN, 3, Scalar(255, 255, 255), 2);
      }
    }
    else
      for (int i = 0; i < result.size(); ++i)
      {
        circle(*_input->GetColorOverlayImage(), result.at(i).GetCenter(), (int)result.at(i).GetRadius() + 2, _input->GetTrafficLightResults()->_colors[result.at(i).GetColor()], 2);
        putText(*_input->GetColorOverlayImage(), _input->GetTrafficLightResults()->_names[result.at(i).GetColor()] + "TrafficLight" + to_string(i), Point(result.at(i).GetCenter().x - (int)result.at(i).GetRadius(), result.at(i).GetCenter().y - (int)result.at(i).GetRadius()), FONT_HERSHEY_PLAIN, 1, _input->GetTrafficLightResults()->_colors[result.at(i).GetColor()]);
      }
  }

  void Display::DrawStopSign()
  {
    Vision::StopSign::Data result = *_input->GetStopSignResults();
    circle(*_input->GetColorOverlayImage(), result.GetRegion()._center, (int)result.GetRegion()._radius + 2, Scalar(0, 255, 255));
    putText(*_input->GetColorOverlayImage(), "StopSign", Point(result.GetRegion()._center.x - (int)result.GetRegion()._radius, result.GetRegion()._center.y - (int)result.GetRegion()._radius), FONT_HERSHEY_PLAIN, 1, Scalar(0, 255, 255));
  }

  void Display::DisplayImage()
  {
    if (_input->GetColorOverlayImage()->rows > 0 && _input->GetColorOverlayImage()->cols > 0)
    {
      namedWindow("Color Image", WINDOW_NORMAL);
      moveWindow("Color Image", _params->GetColorWindowPosition().x, _params->GetColorWindowPosition().y);
      resizeWindow("Color Image", (int)(_input->GetColorOverlayImage()->cols * _params->GetColorWindowScale()), (int)(_input->GetColorOverlayImage()->rows * _params->GetColorWindowScale()));
      imshow("Color Image", *_input->GetColorOverlayImage());
      waitKey(1);
    }

    if (_output->GetColorImage()->rows > 0 && _output->GetDepthImage()->cols > 0)
    {
      namedWindow("Depth Image", WINDOW_NORMAL);
      moveWindow("Depth Image", _params->GetDepthWindowPosition().x, _params->GetDepthWindowPosition().y);
      resizeWindow("Depth Image", (int)(_input->GetDepthOverlayImage()->cols * _params->GetDepthWindowScale()), (int)(_input->GetDepthOverlayImage()->rows * _params->GetDepthWindowScale()));
      imshow("Depth Image", *_input->GetDepthOverlayImage());
      waitKey(1);
    }
  }
}