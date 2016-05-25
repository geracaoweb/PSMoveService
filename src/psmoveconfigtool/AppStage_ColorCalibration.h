#ifndef APP_STAGE_COLOR_CALIBRATION_H
#define APP_STAGE_COLOR_CALIBRATION_H

//-- includes -----
#include "AppStage.h"
#include "ClientPSMoveAPI.h"

#include <vector>

//-- definitions -----
class AppStage_ColorCalibration : public AppStage
{
public:
	AppStage_ColorCalibration(class App *app);

	virtual void enter() override;
	virtual void exit() override;
	virtual void update() override;
	virtual void render() override;

	virtual void renderUI() override;

	static const char *APP_STAGE_NAME;

	void request_tracker_start_stream(int trackerID);
	void request_tracker_stop_stream(int trackerID);
	void request_tracker_set_exposure(int trackerID, double value);
	void request_tracker_get_settings(int trackerID);

protected:
	static void handle_tracker_start_stream_response(
		const ClientPSMoveAPI::ResponseMessage *response,
		void *userdata);
	void open_shared_memory_stream();

	static void handle_tracker_stop_stream_response(
		const ClientPSMoveAPI::ResponseMessage *response,
		void *userdata);
	void close_shared_memory_stream();

	static void handle_tracker_set_exposure_response(
		const ClientPSMoveAPI::ResponseMessage *response,
		void *userdata);
	static void handle_tracker_get_settings_response(
		const ClientPSMoveAPI::ResponseMessage *response,
		void *userdata);

private:
	enum eTrackerMenuState
	{
		inactive,
		idle,

		pendingTrackerStartStreamRequest,
		failedTrackerStartStreamRequest,

		pendingTrackerStopStreamRequest,
		failedTrackerStopStreamRequest,
	};

	eTrackerMenuState m_menuState;
	bool m_bStreamIsActive;
	class ClientTrackerView *m_tracker_view;
	class TextureAsset *m_video_texture;
	double m_trackerExposure;
};

#endif // APP_STAGE_COLOR_CALIBRATION_H