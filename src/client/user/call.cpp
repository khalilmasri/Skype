#include "call.hpp"
#include "peer_to_peer.hpp"
#include "logger.hpp"
#include "job.hpp"
#include "job_bus.hpp"

#include <string>

#define TIMEOUT 10

void Call::connect(Job &t_job)
{
  P2P call(t_job.m_argument);

  std::string id = std::to_string(t_job.m_intValue);
  call.connect_peer(id);

  if (call.status() != P2P::Awaiting) {
    LOG_ERR("did not call connect correctly. Exiting...");
    return;
  }

  int count = 0;

  while (call.status() != P2P::Accepted) {

    std::cout << "Pinging ...\n";

    call.ping_peer();
    sleep(1); // check every  2 second

    if (call.status() == P2P::Awaiting) {
      LOG_DEBUG("Still Awaiting...");
    }
    
    if (call.status() == P2P::Error) {
      LOG_ERR("ping returned an error. exiting....");
      return;
    }

    if ( true == m_hangup)
    {
      LOG_DEBUG("Hanging up the call!");
      m_hangup = false;
      return;
    }

    if (count > TIMEOUT) {
      LOG_INFO("Breaking after %d seconds", count);
      JobBus::create({Job::HANGUP});
      call.hangup_peer();
      return;
    }

    count++;
  }

  LOG_INFO("Call accepted");
  call.handshake_peer();
}

void Call::accept(Job &t_job)
{
  
  P2P call(t_job.m_argument);
  
  std::string id = std::to_string(t_job.m_intValue);
  call.accept_peer(id);

  if (call.status() == P2P::Accepted) {
    LOG_INFO("Accepted was sucessful");
  } else {
    LOG_ERR("Accept failed.");
  }

  LOG_INFO("Call accepted");
  call.handshake_peer();
}

void Call::reject(Job &t_job)
{
  P2P call(t_job.m_argument);

  std::string id = std::to_string(t_job.m_intValue);
  call.reject_peer(id);
}

void Call::hangup()
{
  m_hangup = true;
}

void Call::mute()
{
  if ( false == m_mute)
  {
    LOG_INFO("Mute ON");
    m_mute = true;
  }else{
    LOG_INFO("Mute OFF");
    m_mute = false;
  }
}

void Call::webcam()
{
  if ( false == m_webcam)
  {
    LOG_INFO("Turning webcam");
    m_webcam = true;
  }else{
    LOG_INFO("Closing webcam");
    m_webcam = false;
  }
}
