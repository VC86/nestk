#ifndef ADSOBJECTTRACKER_H
#define ADSOBJECTTRACKER_H

#include "object_detector.h"
#include "object_match.h"
#include <ntk/geometry/pose_3d.h>
#include <opencv2/video/tracking.hpp>

namespace ntk
{

class ObjectTracker
{
public:
  ObjectTracker(const ObjectMatch& match);

  int numFramesWithoutDetection() const { return m_nframes_without_detection; }
  int numFramesWithDetection() const { return m_nframes_with_detection; }

  void prepareForNewFrame();
  void addNewDetection(const ObjectMatch& match);
  double compatibilityMeasure(const ObjectMatch& match) const;
  bool hasUpdatedPose() const { return m_has_updated_pose; }
  const ntk::Pose3D& previousObjectPose() const { return m_last_pose; }
  const ntk::Pose3D& objectPose() const { return m_estimated_pose; }
  const ntk::Pose3D& rawObjectPose() const { return m_raw_pose; }
  const VisualObject* objectModel() const { return m_model; }
  const cv::Rect& objectBoundingRect() const { return m_last_projected_bounding_rect; }

private:
  ntk::Pose3D m_last_pose;
  ntk::Pose3D m_estimated_pose;
  ntk::Pose3D m_raw_pose;
  const VisualObject* m_model;
  cv::Rect m_last_projected_bounding_rect;
  int m_nframes_without_detection;
  int m_nframes_with_detection;
  bool m_has_updated_pose;
  cv::KalmanFilter m_kalman;
};
ntk_ptr_typedefs(ObjectTracker)

} // avs

#endif // ADSOBJECTTRACKER_H
