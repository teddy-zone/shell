#pragma once

#include "component.h"

#include "voice_component.h"

enum class WeatherState
{
	None,
	Snow,
	Rain,
    PartlyCloudy,
    Dusk
};

struct CompWeather : public Component
{
	WeatherState state = WeatherState::None;
    WeatherState prev_state = WeatherState::None;

    ~CompWeather()
    {
        std::cout << "Deleted it!" << "\n";
    }
};

struct WeatherEntityProto : public EntityProto
{

    WeatherEntityProto(const std::vector<CompType>& extension_types = {}) :
        EntityProto(extension_types)
    {
        std::vector<CompType> unit_components = { {
                    uint32_t(type_id<CompWeather>),
                    uint32_t(type_id<CompPosition>),
                    uint32_t(type_id<CompVoice>),
                    uint32_t(type_id<CompPointLight>),
            } };
        append_components(unit_components);
    }

    virtual void init(EntityRef entity, SystemInterface* iface) override
    {

        Sound wind_sound;
        wind_sound.path = "sounds\\wind.mp3";
        wind_sound.loop = true;
        wind_sound.trigger = true;
        wind_sound.range = 100;
        wind_sound.volume = 0.15;
        wind_sound.spatial = false;
        entity.cmp<CompVoice>()->sounds["wind"] = wind_sound;
        //entity.cmp<CompWeather>()->state = WeatherState::Rain;

        Sound rain_sound;
        rain_sound.path = "sounds\\rain.mp3";
        rain_sound.loop = true;
        rain_sound.trigger = true;
        rain_sound.range = 100;
        rain_sound.volume = 0.65;
        rain_sound.spatial = false;
        rain_sound.sound_name = "rain";
        entity.cmp<CompVoice>()->sounds["rain"] = rain_sound;

        Sound cicada_sound;
        cicada_sound.path = "sounds\\cicada.wav";
        cicada_sound.loop = true;
        cicada_sound.trigger = true;
        cicada_sound.range = 100;
        cicada_sound.volume = 0.55;
        cicada_sound.spatial = false;
        entity.cmp<CompVoice>()->sounds["cicada"] = cicada_sound;

        entity.cmp<CompWeather>()->state = WeatherState::Rain;
    }
};