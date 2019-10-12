/*******************************************************************************
 *
 * File: SubscriptionStore.h
 *
 * This file contains the definition of several classes that can be used
 * to implement a publish/subscibe data sharing interface through a static
 * management object (the SubscriptionStore).
 *
 * Classes that want to publish data should create an instance of the 
 * SSPublisher class, call SubscriptionStore::registerPublisher(), then
 * just call that classes publish() method each time there is data to
 * be published.
 *      SSPublisher<Point3d<float> > publisher;
 *		SubscriptionStore::registerPublisher<Point3d<float> >("test_topic", &publisher);
 *      publisher.publish(data);
 *
 * Classes that want to subscribe to the data just need to call the
 * SubscriptionStore::subscribe() method with a method that can 
 * handle the data type.
 *		my_id = SubscriptionStore::subscribe("test_topic",
 *			(std::function<void(Point3d<float>)>)(std::bind(
 *			&SubscriptionStoreTest::myCallback, this, std::placeholders::_1)));
 *
 * NOTICE: All processing in the callback is done on the publishers thread.
 *         So, at least for now, the callback function should be designed 
 *         to be extremely limitted in what it does with the data, just 
 *         copy the data and maybe set a flag, or just push the value
 *         to a queue. 
 *
 * Classes that are finished with the data can unsubscribe to stop 
 * getting messages. 
 *      SubscriptionStore::unsubscribe(my_id);
 *
 * Written by:
 * 	The Robonauts
 * 	FRC Team 118
 * 	NASA, Johnson Space Center
 * 	Clear Creek Independent School District
 *
 ******************************************************************************/
#pragma once

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <cmath>

#include <map>
#include <vector>
#include <functional>

#include <iostream>

#include <gsi/Mutex.h>

#define VOID_SUBSCRIPTION_ID 0

namespace gsu 
{

/***************************************************************************//**
 *
 * This base class is just a type that can be put into the SubscriptionStore's
 * map.
 *
 ******************************************************************************/
class SSPublisherBase
{
	public:
		virtual void unsubscribe(uint32_t subscription_id) = 0;
		virtual std::string toString(void) = 0;

	protected:
		SSPublisherBase(void) {}
		virtual ~SSPublisherBase(void) {}
};

/***************************************************************************//**
 *
 * The templated publisher class manages all subscribers just create an
 * instance of this class to publish any data type.
 *
 ******************************************************************************/
template <class T>
class SSPublisher : public SSPublisherBase
{
    public:
		uint32_t subscribe(std::function<void(T)> callback, uint32_t subscription_id = VOID_SUBSCRIPTION_ID);
		void unsubscribe(uint32_t subscription_id);
		size_t publish(T x);
		std::string toString(void);

    private:
		static std::map<uint32_t, std::function<void(T)> > m_subscriptions;
		static gsi::ExclusiveMutex m_mutex;
};

/***************************************************************************//**
 *
 * The SSVoidPublisher class is a special publisher that doesn't actually
 * publish. The SubscriptionStore will create instances of these if something
 * tries to subscribe to a topic that is not being published. If a publisher
 * registers for the topic at a later time, all of the subscribers to the
 * void publisher are automatically moved to the actual publisher.
 *
 ******************************************************************************/
template <class T>
class SSVoidPublisher : public SSPublisherBase
{
    public:
		uint32_t subscribe(std::function<void(T)> callback, uint32_t subscription_id = VOID_SUBSCRIPTION_ID);
		void unsubscribe(uint32_t subscription_id);
        void transferSubscribers(SSPublisher<T> *new_publisher);
		std::string toString(void);

	private:
		static std::map<uint32_t, std::function<void(T)> > m_subscriptions;
		static gsi::ExclusiveMutex m_mutex;
};

/***************************************************************************//**
 *
 * The SubscribeStore is a purely static class used to manage a list of
 * publishers and to help connect subscribers to the correct publisher.
 *
 ******************************************************************************/
class SubscriptionStore
{
	public:
		template <class T>
		static void registerPublisher(std::string name, SSPublisher<T> *publisher);

		template <class T>
		static uint32_t subscribe(std::string name, std::function<void(T)> funct);

		static void unsubscribe(uint32_t subscription_id);

		static uint32_t getNextSubscriptionId(void);
		static void listPublishers(void);

	private:
		static std::map<std::string, SSPublisherBase *> m_publishers;
		static uint32_t m_last_subscription;
};


/***************************************************************************//**
 *
 ******************************************************************************/
template <class T>
uint32_t SSPublisher<T>::subscribe(std::function<void(T)> callback, uint32_t subscription_id)
{
	if (subscription_id == VOID_SUBSCRIPTION_ID)
	{
		subscription_id = SubscriptionStore::getNextSubscriptionId();
	}

	m_mutex.lock();
	m_subscriptions[subscription_id] = callback;
	m_mutex.unlock();

	return subscription_id;
}

/***************************************************************************//**
 *
 ******************************************************************************/
template <class T>
void SSPublisher<T>::unsubscribe(uint32_t subscription_id)
{
	m_mutex.lock();
	m_subscriptions.erase(subscription_id);
	m_mutex.unlock();
}

/***************************************************************************//**
 *
 ******************************************************************************/
template <class T>
size_t SSPublisher<T>::publish(T x)
{
	m_mutex.lock();
	for (auto subscription : m_subscriptions)
	{
		try
		{
			subscription.second(x);
		}
		catch (...)
		{
			printf("ERROR: %s:%d\n", __FILE__, __LINE__);
		}
	}
	m_mutex.unlock();

	return m_subscriptions.size();
}

/***************************************************************************//**
 *
 ******************************************************************************/
template <class T>
std::string SSPublisher<T>::toString(void)
{
	// NOTE: on some compilers, this name may be mangled, empty, or something else
	return typeid(T).name();
}

/***************************************************************************//**
 *
 ******************************************************************************/
template <class T>
std::map<uint32_t, std::function<void(T)> > SSPublisher<T>::m_subscriptions;

/***************************************************************************//**
 *
 ******************************************************************************/
template <class T>
gsi::ExclusiveMutex SSPublisher<T>::m_mutex;

/***************************************************************************//**
 *
 ******************************************************************************/
template <class T>
uint32_t SSVoidPublisher<T>::subscribe(std::function<void(T)> callback, uint32_t subscription_id)
{
	if (subscription_id == VOID_SUBSCRIPTION_ID)
	{
		subscription_id = SubscriptionStore::getNextSubscriptionId();
	}

	m_mutex.lock();
	m_subscriptions[subscription_id] = callback;
	m_mutex.unlock();

	return subscription_id;
}

/***************************************************************************//**
 *
 ******************************************************************************/
template <class T>
void SSVoidPublisher<T>::unsubscribe(uint32_t subscription_id)
{
	m_mutex.lock();
	m_subscriptions.erase(subscription_id);
	m_mutex.unlock();
}

/***************************************************************************//**
 *
 ******************************************************************************/
template <class T>
void SSVoidPublisher<T>::transferSubscribers(SSPublisher<T> *new_publisher)
{
	if (new_publisher == nullptr)
	{
		return;
	}

	m_mutex.lock();
	for (auto subscription : m_subscriptions)
	{
		new_publisher->subscribe(subscription.second, subscription.first);
	}

	m_subscriptions.clear();
	m_mutex.unlock();
}

/***************************************************************************//**
 *
 ******************************************************************************/
template <class T>
std::string SSVoidPublisher<T>::toString(void)
{
	// NOTE: on some compilers, this name may be mangled, empty, or something else
	return (typeid(T).name());
}

/***************************************************************************//**
 *
 ******************************************************************************/
template <class T>
std::map<uint32_t, std::function<void(T)> > SSVoidPublisher<T>::m_subscriptions;

/***************************************************************************//**
 *
 ******************************************************************************/
template <class T>
gsi::ExclusiveMutex SSVoidPublisher<T>::m_mutex;

/***************************************************************************//**
 *
 * The SubscribeStore is a purely static class used to manage a list of
 * publishers and to help connect subscribers to the correct publisher.
 *
 ******************************************************************************/

template <class T>
void SubscriptionStore::registerPublisher(std::string name, SSPublisher<T> *publisher)
{
	std::map<std::string, SSPublisherBase *>::iterator ittr = m_publishers.find(name);

	if (ittr == m_publishers.end())
	{
		m_publishers[name] = publisher;
	}
	else
	{
		SSVoidPublisher<T> *sub = dynamic_cast<SSVoidPublisher<T> *>(ittr->second);
		if (sub == nullptr)
		{
			printf("registerPublisher error, message type mismatch???  %s:%d\n", __FILE__, __LINE__);
			// error
		}
		else
		{
			sub->transferSubscribers(publisher);
			m_publishers[name] = publisher;
			delete sub;
		}
	}
}

/***************************************************************************//**
 *
 ******************************************************************************/
template <class T>
uint32_t SubscriptionStore::subscribe(std::string name, std::function<void(T)> funct)
{
	std::map<std::string, SSPublisherBase *>::iterator ittr = m_publishers.find(name);

	if (ittr == m_publishers.end())
	{
		SSVoidPublisher<T> *list = new SSVoidPublisher<T>();
		m_publishers[name] = list;
		return list->subscribe(funct, getNextSubscriptionId());
	}
	else
	{
		SSPublisher<T> *pub = dynamic_cast<SSPublisher<T> *>(ittr->second);
		if (pub == nullptr)
		{
			printf("Subscribe error, message type mismatch???  %s:%d\n", __FILE__, __LINE__);
		}
		else
		{
			return pub->subscribe(funct, getNextSubscriptionId());
		}
	}

	return VOID_SUBSCRIPTION_ID;
}

}
