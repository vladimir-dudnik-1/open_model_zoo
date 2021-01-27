/*
// Copyright (C) 2021 Intel Corporation
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
*/
#pragma once
#include "opencv2/core.hpp"
#include "results.h"


struct Peak {
    Peak(const cv::Point2f& keypoint = cv::Point2f(-1, -1),
         const float score = 0.0f,
         const float tag = 0.0f) :
      keypoint(keypoint),
      score(score),
      tag(tag) {}

    cv::Point2f keypoint;
    float score;
    float tag;
};


class Pose {
    public:
        Pose(int numJoints) : peaks(numJoints) {}

        void add(size_t index, Peak peak) {
            peaks[index] = peak;
            sum += peak.score;
            poseTag = (poseTag * static_cast<float>(validPointsNum)) + peak.tag;
            validPointsNum += 1;
            poseTag = poseTag / static_cast<float>(validPointsNum);
        }

        float getPoseTag() const { return poseTag; }

        float getMeanScore() const { return sum / static_cast<float>(validPointsNum); }

        Peak& getPeak(size_t index) { return peaks[index]; }

        size_t size() const { return peaks.size(); }

    private:
        std::vector<Peak> peaks;
        int validPointsNum = 0;
        float poseTag = 0;
        float sum = 0;
};

void findPeaks(const std::vector<cv::Mat>& nmsHeatMaps,
               const std::vector<cv::Mat>& aembdsMaps,
               std::vector<std::vector<Peak>>& allPeaks,
               int jointId, int maxNumPeople,
               float detectionThreshold);

std::vector<Pose> matchByTag(std::vector<std::vector<Peak>>& allPeaks,
                             int maxNumPeople, int numJoints,
                             float tagThreshold,
                             bool useDetectionVal, bool ignoreTooMuch);

void adjustAndRefine(std::vector<Pose>& allPoses,
                     const std::vector<cv::Mat>& heatMaps,
                     const std::vector<cv::Mat>& aembdsMaps,
                     int poseId, float delta,
                     bool doAdjust, bool doRefine);
