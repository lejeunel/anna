#include "eyetribetracker.h"
#include <QDebug>


/*
 * ==============================================================================
 *                              public
 * ==============================================================================
 */
EyeTribeTracker::EyeTribeTracker() {
}

void EyeTribeTracker::connectTracker() {
    if(receiver.connect())
        receiver.add_listener(*this);
    else
        qDebug() << "could not connect to eye tracker";
}

void EyeTribeTracker::disconnectTracker() {
    receiver.remove_listener(*this);
    receiver.disconnect();
//    qDebug() << "disconnectTracker(), receiver " << receiver.is_connected();
}

bool EyeTribeTracker::isConnected() const {
//    return receiver.get_server_state().trackerstate == gtl::ServerState::TRACKER_CONNECTED && connected;
    return receiver.is_connected();
}


/*
 * ==============================================================================
 *                              private
 * ==============================================================================
 */
void EyeTribeTracker::on_gaze_data(const gtl::GazeData &data) {
    if(data.state & gtl::GazeData::GD_STATE_TRACKING_GAZE) {
        TrackingData update;
        // averaged gaze coordinates are more accurate than raw coordinates
        update.x = data.avg.x;
        update.y = data.avg.y;
        update.isFixated = data.fix;

        emit newAnnotationData(update);
    }
}
