// EvolutionCore.h
//

#pragma once

#include "EvolutionTypes.h"

class GridPoint;
class IndId;

class ModelData
{
public:

    virtual ~ModelData( ) { };

    static ModelData * CreateModelData( );

    virtual void           InitEditorState( ) = 0;
    virtual void           CopyModelData  ( ModelData const * const ) = 0;

    virtual int            GetNrOfLivingIndividuals ( ) const = 0;
    virtual int            GetAverageFoodGrowth     ( ) const = 0;
    virtual EVO_GENERATION GetEvoGenerationNr       ( ) const = 0;
    virtual short          GetBrushIntensity        ( ) const = 0;
    virtual tShape         GetBrushShape            ( ) const = 0;
    virtual short          GetBrushSize             ( ) const = 0;
    virtual tBrushMode     GetBrushMode             ( ) const = 0;

    virtual bool           IsDead         ( GridPoint const & ) const = 0;
    virtual bool           IsAlive        ( GridPoint const & ) const = 0;
    virtual IndId          GetId          ( GridPoint const & ) const = 0;
    virtual tOrigin        GetOrigin      ( GridPoint const & ) const = 0;
    virtual short          GetEnergy      ( GridPoint const & ) const = 0;
    virtual int            GetFoodStock   ( GridPoint const & ) const = 0;
    virtual int            GetFertility   ( GridPoint const & ) const = 0;
    virtual int            GetMutationRate( GridPoint const & ) const = 0;
    virtual int            GetFertilizer  ( GridPoint const & ) const = 0;
    virtual EVO_GENERATION GetGenBirth    ( GridPoint const & ) const = 0;
    virtual EVO_GENERATION GetAge         ( GridPoint const & ) const = 0;
    virtual MEM_INDEX      GetMemSize     ( GridPoint const & ) const = 0;
    virtual MEM_INDEX      GetMemUsed     ( GridPoint const & ) const = 0;
    virtual tStrategyId    GetStrategyId  ( GridPoint const & ) const = 0;
    virtual long           GetMemEntry    ( GridPoint const &, MEM_INDEX const ) const = 0;
    virtual short          GetDistr       ( GridPoint const &, tAction   const ) const = 0;
    virtual long           GetGenotype    ( GridPoint const &, tGeneType const ) const = 0;

    virtual GridPoint FindGridPoint( IndId const & ) = 0;

    // manipulating functions

    virtual void SetBrushShape    ( tShape     const ) = 0;
    virtual void SetBrushSize     ( short      const ) = 0;
    virtual void SetBrushIntensity( short      const ) = 0;
    virtual void SetBrushStrategy ( tBrushMode const ) = 0;
    virtual void DoEdit           ( GridPoint  const ) = 0;
};
