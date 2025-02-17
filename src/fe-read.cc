#include "globals.h"
#include "flags.h"
#include "readerwriter.h"
#include "fluxmap.h"
#include "decoders/decoders.h"
#include "macintosh/macintosh.h"
#include "sector.h"
#include "proto.h"
#include "fluxsource/fluxsource.h"
#include "fluxsink/fluxsink.h"
#include "arch/brother/brother.h"
#include "arch/ibm/ibm.h"
#include "imagewriter/imagewriter.h"
#include "fluxengine.h"
#include <google/protobuf/text_format.h>
#include <fstream>

static FlagGroup flags;

static StringFlag sourceFlux({"-s", "--source"},
    "flux file to read from",
    "",
    [](const auto& value)
    {
        globalConfig().setFluxSource(value);
    });

static StringFlag destImage({"-o", "--output"},
    "destination image to write",
    "",
    [](const auto& value)
    {
        globalConfig().setImageWriter(value);
    });

static StringFlag copyFluxTo({"--copy-flux-to"},
    "while reading, copy the read flux to this file",
    "",
    [](const auto& value)
    {
        globalConfig().setCopyFluxTo(value);
    });

static StringFlag srcTracks({"--cylinders", "-c"},
    "tracks to read from",
    "",
    [](const auto& value)
    {
        setRange(globalConfig().overrides()->mutable_tracks(), value);
    });

static StringFlag srcHeads({"--heads", "-h"},
    "heads to read from",
    "",
    [](const auto& value)
    {
        setRange(globalConfig().overrides()->mutable_heads(), value);
    });

int mainRead(int argc, const char* argv[])
{
    if (argc == 1)
        showProfiles("read", formats);
    globalConfig().set("flux_source.type", "FLUXTYPE_DRIVE");
    flags.parseFlagsWithConfigFiles(argc, argv, formats);

    if (globalConfig()->decoder().copy_flux_to().type() == FLUXTYPE_DRIVE)
        error("you cannot copy flux to a hardware device");

    auto& fluxSource = globalConfig().getFluxSource();
    auto& decoder = globalConfig().getDecoder();
    auto writer = globalConfig().getImageWriter();

    readDiskCommand(*fluxSource, *decoder, *writer);

    return 0;
}
