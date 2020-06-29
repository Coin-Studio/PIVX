// Copyright (c) 2009-2010 Satoshi Nakamoto
// Copyright (c) 2009-2016 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef BITCOIN_CONSENSUS_PARAMS_H
#define BITCOIN_CONSENSUS_PARAMS_H

#include "amount.h"
#include "uint256.h"
#include <map>
#include <string>

namespace Consensus {

/**
 * Parameters that influence chain consensus.
 */
struct Params {
    uint256 hashGenesisBlock;
    bool fPowAllowMinDifficultyBlocks;
    uint256 powLimit;
    uint256 posLimit;
    int nBudgetCycleBlocks;
    int nBudgetFeeConfirmations;
    int nCoinbaseMaturity;
    int nMasternodeCountDrift;
    CAmount nMaxMoneyOut;
    int nPoolMaxTransactions;
    int64_t nProposalEstablishmentTime;
    int nStakeMinAge;
    int nStakeMinDepth;
    int64_t nTargetTimespan;
    int64_t nTargetSpacing;
    int nTimeSlotLength;

    // spork keys
    std::string strSporkPubKey;

    // height-based activations
    int height_last_PoW;
    
    int64_t TargetTimespan() const { return nTargetTimespan; }
    uint256 ProofOfStakeLimit() const { return posLimit; }
    bool MoneyRange(const CAmount& nValue) const { return (nValue >= 0 && nValue <= nMaxMoneyOut); }

    int FutureBlockTimeDrift(const int nHeight) const
    {
        // PoS (TimeV2): 14 seconds
        return nTimeSlotLength - 1;
    }

    bool IsValidBlockTimeStamp(const int64_t nTime, const int nHeight) const
    {
        // Time protocol v2 requires time in slots
        return (nTime % nTimeSlotLength) == 0;
    }

    bool HasStakeMinAgeOrDepth(const int contextHeight, const uint32_t contextTime, const int utxoFromBlockHeight, const uint32_t utxoFromBlockTime) const
    {
        return (contextHeight - utxoFromBlockHeight >= nStakeMinDepth);
    }
};
} // namespace Consensus

#endif // BITCOIN_CONSENSUS_PARAMS_H
