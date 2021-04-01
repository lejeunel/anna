/*
 * Communicates with the EyeTribe eye tracker. For more information about the
 * EyeTribe api check out:
 * https://github.com/EyeTribe/documentation#api-reference
 * 18. February 2017, Severin Tobler
 */
#ifndef EYETRIBETRACKER_H
#define EYETRIBETRACKER_H

#include "eyetrackerinterface.h"
#include "eyetribe/gazeapi.hpp"

class EyeTribeTracker : public EyeTrackerInterface, public gtl::IGazeListener {
public:
    EyeTribeTracker();

    void connectTracker() override;
    void disconnectTracker() override;
    bool isConnected() const override;

private:
    void on_gaze_data(gtl::GazeData const & gaze_data) override;

    gtl::GazeApi receiver;
};

#endif // EYETRIBETRACKER_H
