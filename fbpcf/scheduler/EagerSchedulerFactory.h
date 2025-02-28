/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#pragma once

#include "fbpcf/engine/ISecretShareEngineFactory.h"
#include "fbpcf/engine/SecretShareEngineFactory.h"
#include "fbpcf/engine/communication/IPartyCommunicationAgentFactory.h"
#include "fbpcf/scheduler/EagerScheduler.h"
#include "fbpcf/scheduler/ISchedulerFactory.h"
#include "fbpcf/scheduler/WireKeeper.h"

namespace fbpcf::scheduler {

template <bool unsafe>
class EagerSchedulerFactory final : public ISchedulerFactory<unsafe> {
 public:
  EagerSchedulerFactory(
      std::unique_ptr<engine::ISecretShareEngineFactory> engineFactory)
      : engineFactory_(std::move(engineFactory)) {}

  std::unique_ptr<IScheduler> create() override {
    return std::make_unique<EagerScheduler>(
        engineFactory_->create(), WireKeeper::createWithVectorArena<unsafe>());
  }

 private:
  std::unique_ptr<engine::ISecretShareEngineFactory> engineFactory_;
};

template <bool unsafe>
inline std::unique_ptr<EagerSchedulerFactory<unsafe>>
getEagerSchedulerFactoryWithInsecureEngine(
    int myId,
    engine::communication::IPartyCommunicationAgentFactory&
        communicationAgentFactory) {
  std::unique_ptr<engine::ISecretShareEngineFactory> engineFactory =
      engine::getInsecureEngineFactoryWithDummyTupleGenerator(
          myId, 2, communicationAgentFactory);

  return std::make_unique<EagerSchedulerFactory<unsafe>>(
      std::move(engineFactory));
}

inline std::unique_ptr<EagerSchedulerFactory</* unsafe */ true>>
getEagerSchedulerFactoryWithClassicOT(
    int myId,
    engine::communication::IPartyCommunicationAgentFactory&
        communicationAgentFactory) {
  std::unique_ptr<engine::ISecretShareEngineFactory> engineFactory =
      engine::getSecureEngineFactoryWithClassicOt<bool>(
          myId, 2, communicationAgentFactory);

  return std::make_unique<EagerSchedulerFactory</* unsafe */ true>>(
      std::move(engineFactory));
}

inline std::unique_ptr<EagerSchedulerFactory</* unsafe */ true>>
getEagerSchedulerFactoryWithRealEngine(
    int myId,
    engine::communication::IPartyCommunicationAgentFactory&
        communicationAgentFactory) {
  std::unique_ptr<engine::ISecretShareEngineFactory> engineFactory =
      engine::getSecureEngineFactoryWithFERRET<bool>(
          myId, 2, communicationAgentFactory);

  return std::make_unique<EagerSchedulerFactory</* unsafe */ true>>(
      std::move(engineFactory));
}

} // namespace fbpcf::scheduler
