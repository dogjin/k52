/*
 * CountFitnessTask.cpp
 *
 *  Created on: Mar 11, 2012
 *      Author: feanor
 */

#include "CountObjectiveFunctionTask.h"
#include "ObjectiveFunctionTaskResult.h"

#ifdef BUILD_WITH_MPI

#include <boost/mpi.hpp>
#include <parallel/mpi/Constants.h>
#include <parallel/mpi/IdentifyableObjectsManager.h>

#endif

namespace k52
{
namespace optimize
{

#ifdef BUILD_WITH_MPI

k52::parallel::mpi::IMpiTaskResult::shared_ptr CountObjectiveFunctionTask::createEmptyResult() const
{
	return ObjectiveFunctionTaskResult::shared_ptr ( new ObjectiveFunctionTaskResult() );
}

k52::parallel::mpi::IMpiTaskResult::shared_ptr CountObjectiveFunctionTask::performMpi() const
{
	double value = getFunction()->operator ()( getParameters() );
	ObjectiveFunctionTaskResult::shared_ptr result ( new ObjectiveFunctionTaskResult() );
	result->setObjectiveFunctionValue(value);
	return result;
}

CountObjectiveFunctionTask* CountObjectiveFunctionTask::clone() const
{
	CountObjectiveFunctionTask* clone = NULL;
	if(_wasCreated)
	{
		clone = new CountObjectiveFunctionTask(_parameters, _functionToOptimize);
	}
	else
	{
		clone = new CountObjectiveFunctionTask();
		if(_receivedFunction != NULL)
		{
			clone->_receivedFunction = IObjectiveFunction::shared_ptr( _receivedFunction->clone() );
		}
		if(_receivedParameters != NULL)
		{
			clone->_receivedParameters = IParameters::shared_ptr( _receivedParameters->clone() );
		}
	}
	return clone;
}

void CountObjectiveFunctionTask::send(boost::mpi::communicator* communicator, int target) const
{
	communicator->send(target, k52::parallel::mpi::Constants::CommonTag, k52::parallel::mpi::IdentifyableObjectsManager::getId(getFunction()));
	communicator->send(target, k52::parallel::mpi::Constants::CommonTag, k52::parallel::mpi::IdentifyableObjectsManager::getId(getParameters()));
	getParameters()->send(communicator, target);
}

void CountObjectiveFunctionTask::receive(boost::mpi::communicator* communicator)
{
	std::string objectiveFunctionId;
	communicator->recv(k52::parallel::mpi::Constants::ServerRank, k52::parallel::mpi::Constants::CommonTag, objectiveFunctionId);
	const IObjectiveFunction* functionToOptimize = dynamic_cast<const IObjectiveFunction*>( k52::parallel::mpi::IdentifyableObjectsManager::Instance().getObject(objectiveFunctionId) );
	_receivedFunction = IObjectiveFunction::shared_ptr( functionToOptimize->clone() );

	std::string parametersId;
	communicator->recv(k52::parallel::mpi::Constants::ServerRank, k52::parallel::mpi::Constants::CommonTag, parametersId);
	const IParameters* parameters = dynamic_cast<const IParameters*>( k52::parallel::mpi::IdentifyableObjectsManager::Instance().getObject(parametersId) );
	_receivedParameters = IParameters::shared_ptr( parameters->clone() );
	_receivedParameters->receive(communicator);
}
#else

//TODO use polymorphysm and fix conflict with base class
k52::parallel::ITaskResult::shared_ptr CountObjectiveFunctionTask::perform() const
{
	double value = _functionToOptimize->operator ()(_parameters);
	ObjectiveFunctionTaskResult::shared_ptr result ( new ObjectiveFunctionTaskResult() );
	result->setObjectiveFunctionValue(value);
	return result;
}

#endif

const IObjectiveFunction* CountObjectiveFunctionTask::getFunction() const
{
	if(_wasCreated)
	{
		return _functionToOptimize;
	}
	else
	{
		return _receivedFunction.get();
	}
}

const IParameters* CountObjectiveFunctionTask::getParameters() const
{
	if(_wasCreated)
	{
		return _parameters;
	}
	else
	{
		return _receivedParameters.get();
	}
}


CountObjectiveFunctionTask::CountObjectiveFunctionTask()
	:_parameters(NULL), _functionToOptimize(NULL), _wasCreated(false) {}

CountObjectiveFunctionTask::CountObjectiveFunctionTask(
		const IParameters*  parameters,
		const IObjectiveFunction* functionToOptimize)
			: _parameters(parameters), _functionToOptimize(functionToOptimize), _wasCreated(true) {}

}/* namespace optimize */
}/* namespace k52 */
