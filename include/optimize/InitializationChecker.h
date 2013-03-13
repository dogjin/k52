/*
 * InitializationChecker.h
 *
 *  Created on: Apr 22, 2012
 *      Author: feanor
 */

#ifndef INITIALIZATIONCHECKER_H_
#define INITIALIZATIONCHECKER_H_

//TODO make private or common

/**
 @class Individual
Manages initialization checking process.
Has two states - not initialized and initialized.
 */
class InitializationChecker
{
public:
	///InitializationChecker is created in not initialized state
	InitializationChecker();

	///Checks for being in initialized state.
	///If not in initialized state - throws an logic_error exception.
	void initializationCheck() const;

	///Sets InitializationChecker into initialized state.
	///If already in initialized state - throws an logic_error exception.
	void setInitialized();

private:

	bool _wasInitialized;
};

#endif /* INITIALIZATIONCHECKER_H_ */