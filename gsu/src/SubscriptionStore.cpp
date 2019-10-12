/*******************************************************************************
 *
 * File: SubscriptionStore.cpp
 *
 * Written by:
 * 	The Robonauts
 * 	FRC Team 118
 * 	NASA, Johnson Space Center
 * 	Clear Creek Independent School District
 *
 ******************************************************************************/

#include "gsu/SubscriptionStore.h"

namespace gsu
{

/***************************************************************************//**
 *
 * Instantiate the SubsriptionStore's container for holding publishers.
 *
 ******************************************************************************/
std::map<std::string, SSPublisherBase *> SubscriptionStore::m_publishers;

uint32_t SubscriptionStore::m_last_subscription = VOID_SUBSCRIPTION_ID;

/***************************************************************************//**
 *
 ******************************************************************************/
void SubscriptionStore::unsubscribe(uint32_t subscription_id)
{
	for (auto publisher : m_publishers)
	{
		publisher.second->unsubscribe(subscription_id);
	}
}

/***************************************************************************//**
 *
 ******************************************************************************/
uint32_t SubscriptionStore::getNextSubscriptionId(void)
{
	m_last_subscription++;
	return (m_last_subscription);
}

/***************************************************************************//**
 *
 ******************************************************************************/
void SubscriptionStore::listPublishers(void)
{
	for (auto publisher : m_publishers)
	{
		printf("%s[%s]\n", publisher.first.c_str(), publisher.second->toString().c_str());
	}
}


}
