/*
 * CompositeParameters.cpp
 *
 *  Created on: Apr 22, 2012
 *      Author: feanor
 */

#include <optimize/DiscreteParametersLocalStorage.h>
#include <optimize/params/CompositeDiscreteParameters.h>
#include <stdexcept>

void CompositeDiscreteParameters::initialize(vector<const IDiscreteParameters*> parameters)
{
	_parametersSet = vector<DiscreteParametersLocalStorage>(parameters.size());

	for(size_t i=0; i<_parametersSet.size(); i++)
	{
		_parametersSet[i] = DiscreteParametersLocalStorage(parameters[i]);
	}

	this->setConstChromosomeSize( countTotalChromosomeSize() );

	_initializationChecker.setInitialized();
}

size_t CompositeDiscreteParameters::getNumberOfParameters() const
{
	_initializationChecker.initializationCheck();

    return _parametersSet.size();
}

const IDiscreteParameters* CompositeDiscreteParameters::getParameter(size_t index) const
{
	if(index+1 > _parametersSet.size())
	{
		throw std::out_of_range("index in getParameter method is out of range");
	}

	return _parametersSet[index].getLocalParameters();
}

CompositeDiscreteParameters *CompositeDiscreteParameters::clone() const
{
	_initializationChecker.initializationCheck();

	vector<const IDiscreteParameters*> parameters (_parametersSet.size());

	for(size_t i=0; i<_parametersSet.size(); i++)
	{
		parameters[i] = _parametersSet[i].getLocalParameters();
	}

	CompositeDiscreteParameters* clone = new CompositeDiscreteParameters();
	clone->initialize(parameters);
	return clone;
}

bool CompositeDiscreteParameters::checkConstraints() const
{
	_initializationChecker.initializationCheck();

	for(size_t i=0; i<_parametersSet.size(); i++)
	{
		if(!_parametersSet[i].getLocalParameters()->checkConstraints())
		{
			return false;
		}
	}
	return true;
}

void CompositeDiscreteParameters::setChromosome(vector<bool>::iterator from, vector<bool>::iterator to) const
{
	_initializationChecker.initializationCheck();

	size_t chromosomeSize = to - from;

	this->checkForConstChromosomeSize(chromosomeSize);

	vector<bool>::iterator currentFrom = from;

	for(size_t i = 0; i < _parametersSet.size(); i++)
	{
		size_t parameterChromosomeSize = _parametersSet[i].getChromosomeSize();

		vector<bool>::iterator currentTo = currentFrom + parameterChromosomeSize;

		_parametersSet[i].setChromosome(currentFrom, currentTo);

		currentFrom = currentTo;
	}
}

void CompositeDiscreteParameters::setFromChromosome(vector<bool>::const_iterator from, vector<bool>::const_iterator to)
{
	_initializationChecker.initializationCheck();

	size_t chromosomeSize = to - from;

	this->checkForConstChromosomeSize(chromosomeSize);

	vector<bool>::const_iterator currentFrom = from;

	for(size_t i = 0; i < _parametersSet.size(); i++)
	{
		size_t parameterChromosomeSize = _parametersSet[i].getChromosomeSize();

		vector<bool>::const_iterator currentTo = currentFrom + parameterChromosomeSize;

		_parametersSet[i].setFromChromosome(currentFrom, currentTo);

		currentFrom = currentTo;
	}
}

size_t CompositeDiscreteParameters::countTotalChromosomeSize() const
{
	size_t totalChromosomeSize = 0;
	for(size_t i=0; i<_parametersSet.size(); i++)
	{
		totalChromosomeSize += _parametersSet[i].getChromosomeSize();
	}
	return totalChromosomeSize;
}
