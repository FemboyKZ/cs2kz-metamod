#include "kz_style.h"

#define STYLE_NAME       "LowGravity"
#define STYLE_NAME_SHORT "LWG"

class KZLowGravityStylePlugin : public ISmmPlugin, public IMetamodListener
{
public:
	bool Load(PluginId id, ISmmAPI *ismm, char *error, size_t maxlen, bool late);
	bool Unload(char *error, size_t maxlen);
	bool Pause(char *error, size_t maxlen);
	bool Unpause(char *error, size_t maxlen);

public:
	const char *GetAuthor();
	const char *GetName();
	const char *GetDescription();
	const char *GetURL();
	const char *GetLicense();
	const char *GetVersion();
	const char *GetDate();
	const char *GetLogTag();
};

class KZLowGravityStyleService : public KZStyleService
{
	using KZStyleService::KZStyleService;

public:
	virtual const char *GetStyleName() override
	{
		return "LowGravity";
	}

	virtual const char *GetStyleShortName() override
	{
		return "LWG";
	}

	virtual void Init() override;
	virtual void Cleanup() override;
};
