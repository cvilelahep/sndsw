# SND@LHC Analysis software

This is where the code for running SND@LHC analyses lives. There is an event filter executable `sndEventFilter` that can be used to process events, producing a file with a subselection of events. The processes to be run are defined by "pipeline" files that are interpreted at runtime, i.e., which cuts to apply and the parameters of the cuts can be changed without recompiling the code. There are also processes which do not apply cuts, for example, the SciFi hit filter, which removes SciFi hits that are not in time with the event.

Libraries of generally useful tools that can be used within the event filter or in independent code are also available.

## Directory structure
- analyses: this is where code specific to different analyses is found. For example, the "pipelines" for running the event filter for each sleection are in this directory.
- core: event filter executable and base class for event processor.
- cuts: classes implementing cuts.
- processes: classes implementing processes that aren't cuts.
- tools: general tool libraries
- scripts: other scripts
