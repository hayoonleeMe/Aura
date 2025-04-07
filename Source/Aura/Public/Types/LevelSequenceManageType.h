#pragma once

// Level Sequence가 멈추면 Broadcast하고 해당 Level Sequence Tag를 전달한다.
DECLARE_MULTICAST_DELEGATE_OneParam(FOnLevelSequenceStopSignature, const FName& /*LevelSequenceTag*/);
