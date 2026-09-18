#include "BufferedSound.h"

SoundFile soundFiles[] = {
	// Tonatiuh
	{ "/tepeyo/snd/Tbois.snd", 10340, 0 },
	{ "/tepeyo/snd/Tcailloux.snd", 1896, 0 },
	{ "", 0, 0 }, //{ "Tpierre.snd", 13064, 0 },
	{ "/tepeyo/snd/Tterre.snd", 2196, 0 },

	{ "/tepeyo/snd/Tdefusion.snd", 44034, 0 },
	{ "/tepeyo/snd/Tfusion.snd", 26764, 0 },
	{ "/tepeyo/snd/Tfusionfinale.snd", 176036, 0 },

	{ "/tepeyo/snd/Tchute.snd", 11004, 0 },
	{ "/tepeyo/snd/Tcorne.snd", 352258, 0 },
	{ "/tepeyo/snd/Tmeteorite.snd", 142402, 0 },
	{ "/tepeyo/snd/Tteleport.snd", 17488, 0 },
	{ "/tepeyo/snd/Trespiration.snd", 62916, 0 },

	// Tonatiuh fusionné
	{ "/tepeyo/snd/TRbois.snd", 11458, 0 },
	{ "/tepeyo/snd/TRcailloux.snd", 1436, 0 },
	{ "/tepeyo/snd/TRinerte.snd", 18878, 0 },
	{ "", 0, 0 }, //{ "TRpierre.snd", 20484, 0 },
	{ "/tepeyo/snd/TRterre.snd", 4430, 0 },

	// Relic
	{ "/tepeyo/snd/Ratterit.snd", 27342, 0 },
	{ "/tepeyo/snd/Rcharge.snd", 49390, 0 },
	{ "/tepeyo/snd/Rglisse.snd", 54726, 0 },
	{ "/tepeyo/snd/Rhand.snd", 187012, 0 },
	{ "/tepeyo/snd/Rjette.snd", 24590, 0 },
	{ "/tepeyo/snd/RposeAutel.snd", 44034, 0 },
	{ "", 0, 0 }, // { "Rprend.snd", 22046, 0 },
	{ "/tepeyo/snd/Rrecharge.snd", 197810, 0 },
	{ "/tepeyo/snd/Rrotation.snd", 19810, 0 },
	
	// Altar
	{ "/tepeyo/snd/Acloche0.snd", 352256, 0 },
	{ "/tepeyo/snd/Acloche1.snd", 352264, 0 },
	{ "/tepeyo/snd/Acloche2.snd", 352256, 0 },
	{ "/tepeyo/snd/Acloche3.snd", 352256, 0 },
	{ "/tepeyo/snd/Aenergie.snd", 349850, 0 },

	// Door
	{ "/tepeyo/snd/Door.snd", 122824, 0 },

	// Berserk
	{ "/tepeyo/snd/Tberserkpas.snd", 33038, 0 },

	// Captain' Choc !
	{ "/tepeyo/snd/choc02.snd", 69844, 0 },

	// Tutos
	{ "/tepeyo/snd/affiche.snd", 44034, 0 },

	// La vraie fusion finale
	{ "/tepeyo/snd/Tfusionfinale1.snd", 95108, 0 },
	{ "/tepeyo/snd/Tfusionfinale2.snd", 88066, 0 },
	{ "/tepeyo/snd/Tfusionfinale3.snd", 154114, 0 },
};
