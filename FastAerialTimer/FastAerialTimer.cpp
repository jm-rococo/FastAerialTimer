#include "FastAerialTimer.h"

BAKKESMOD_PLUGIN(FastAerialTimer, "FastAerialTimer", "0.1", PLUGINTYPE_FREEPLAY)


void FastAerialTimer::onLoad()
{
	if (!gameWrapper->IsInFreeplay()) return;
	
	cvarManager->registerCvar("reset_all_time_best", "0", "Reset All Time Best");
	cvarManager->registerNotifier("reset_all_time_best", std::bind(&FastAerialTimer::resetAllTimeBest, this), "reset all time best", PERMISSION_FREEPLAY);

	this->AllTimeBestFile.open(this->savefile_path);
	if (this->AllTimeBestFile.good()) {
		AllTimeBestFile >> this->all_time_best;
		AllTimeBestFile.close();
	}

	gameWrapper->LogToChatbox("Fast Aerial Timer Loaded");
	
	gameWrapper->HookEvent("Function TAGame.Car_TA.OnJumpPressed", std::bind(&FastAerialTimer::onJump, this));
	gameWrapper->HookEvent("Function TAGame.Car_TA.OnRigidBodyCollision", std::bind(&FastAerialTimer::onCollision, this));
}

void FastAerialTimer::onUnload()
{
	gameWrapper->LogToChatbox("Fast Aerial Timer Unloaded");
}


void FastAerialTimer::onJump()
{	
	CarWrapper car = gameWrapper->GetLocalCar();
	if (car.GetLocation().Z < 17.1) {

		gameWrapper->LogToChatbox("Fast Aerial Timer Started...hurry!");
		this->t0 = gameWrapper->GetGameEventAsServer().GetSecondsElapsed();
		this->timing_started = true;
	
	}
}

void FastAerialTimer::onCollision()
{
	CarWrapper car = gameWrapper->GetLocalCar();

	if (car.GetLocation().Z > 1971 & this->timing_started) {
		
		auto duration = gameWrapper->GetGameEventAsServer().GetSecondsElapsed() - t0;

		if (this->best_session < 0 | duration < this->best_session) this->best_session = duration;
		
		if (duration < this->all_time_best | this->all_time_best < 0) this->saveBestTime(duration);

		if (this->all_time_best > -1) gameWrapper->LogToChatbox("Time to ceiling: " + to_string_with_precision(duration, 3) + " s. Best time: " + to_string_with_precision(this->best_session, 3) + " s. All Time Best: " + to_string_with_precision(this->all_time_best, 3) + " s");
		else gameWrapper->LogToChatbox("Time to ceiling: " + std::to_string(duration) + " ms. Best time: " + std::to_string(this->best_session) + " ms.");

		this->timing_started = false;
		
	}
}

void FastAerialTimer::saveBestTime(float time)
{
	this->all_time_best = time;
	this->AllTimeBestFile.open(this->savefile_path, std::ios::out | std::ios::trunc);
	if (this->AllTimeBestFile.good()) {
		AllTimeBestFile << time;
		this->AllTimeBestFile.close(); 
	}
}

void FastAerialTimer::resetAllTimeBest()
{
	this->AllTimeBestFile.open(this->savefile_path, std::ios::out | std::ios::trunc);
	gameWrapper->LogToChatbox("All Time Best has been reset");
	if (this->AllTimeBestFile.good()) {
		this->AllTimeBestFile.close();
	}
	this->all_time_best = -1;

}