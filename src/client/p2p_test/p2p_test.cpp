#define DOCTEST_CONFIG_IMPLEMENT

#include "active_conn.hpp"
#include "audio_converter.hpp"
#include "audio_device.hpp"
#include "audio_device_config.hpp"
#include "av_playback.hpp"
#include "av_stream.hpp"
#include "config.hpp"
#include "doctest.h"
#include "lock_free_audio_queue.hpp"
#include "peer_to_peer.hpp"
#include "request.hpp"
#include "string_utils.hpp"
#include "text_data.hpp"
#include "text_io.hpp"
#include "webcam.hpp"
#include <iostream>
#include <thread>
#include <chrono>
#include "audio_bank.hpp"

#include <stdio.h>

typedef struct wav_header {
    // RIFF Header
    char riff_header[4]; // Contains "RIFF"
    int wav_size; // Size of the wav portion of the file, which follows the first 8 bytes. File size - 8
    char wave_header[4]; // Contains "WAVE"
    
    // Format Header
    char fmt_header[4]; // Contains "fmt " (includes trailing space)
    int fmt_chunk_size; // Should be 16 for PCM
    short audio_format; // Should be 1 for PCM. 3 for IEEE Float
    short num_channels;
    int sample_rate;
    int byte_rate; // Number of bytes per second. sample_rate * num_channels * Bytes Per Sample
    short sample_alignment; // num_channels * Bytes Per Sample
    short bit_depth; // Number of bits per sample
    
    // Data
    char data_header[4]; // Contains "data"
    int data_bytes; // Number of bytes in data. Number of samples * num_channels * sample byte size
    // uint8_t bytes[]; // Remainder of wave file is bytes
} wav_header_t;


static Config *config = Config::get_instance();

/* Tests */
void test_stream(char *user);
void test_audio();
void test_video();
void write_wav();
P2P test_conn(char *t_user, char *t_pw);
auto callback(std::unique_ptr<P2P> &p2p) -> AVStream::DataCallback;

/* helpers */
auto login_as(ActiveConn &conn, const char *user, const char *password)
    -> std::string;
void logout(ActiveConn &conn, std::string &token);
void connect_to(P2P &p2p);
void accept_from(P2P &p2p);
void free_configs();

static Request audio_req;
static Request video_req;

/**** Main *****/

auto main(int argc, char **argv) -> int {

  if (argc < 3) {
    return 0;
    std::cout << "Enter a username and password.\n ./p2p_test john 1234\n";
  }

  if (std::string(argv[1]) == "audio") {
    test_audio();
    return 0;
  }

 if (std::string(argv[1]) == "wav") {
    write_wav();
    return 0;
  }


  if (std::string(argv[1]) == "camera") {
    test_video();
    return 0;
  }

  if (std::string(argv[1]) == "stream") {
    test_stream(argv[2]);
    return 0;
  }
  test_conn(argv[1], argv[2]);
  return 0;
}

/*********/

/* Tests Stream
 *  ./build/bin/p2p_test audio -
 * */

void test_stream(char *user) {
  using namespace std::chrono_literals;

  char password[] = "1234";
  std::string u(user);
  P2P p2p = test_conn(user, password);
  auto p2p_ptr = std::make_unique<P2P>(p2p);

  auto stream = AVStream();
  auto playback = AVPlayback();

  // this callback is just to stop the stream on another thread.
  auto stop = [&u, &stream, &playback]() { 
    std::this_thread::sleep_for(20s); // do it for 10 secs then quit.

    if(u == "john"){
      stream.stop();
    }

    if(u == "shakira"){
      playback.stop();
    }
  };

  std::thread thread (stop);

   
 // john streams out
  if(u == "john") {
    auto cb = callback(p2p_ptr);
    stream.start();
    stream.stream(cb);
  }

  // shakira receives
  if(u == "shakira"){
    playback.buffer(p2p_ptr, 10);
    playback.start(p2p_ptr);
  }

  thread.join();
}

auto callback(std::unique_ptr<P2P> &p2p) -> AVStream::DataCallback {

  
  return [&p2p](Webcam::WebcamFrames &&t_video, Data::DataVector &&t_audio) {

   audio_req = p2p->make_request();
   video_req = p2p->make_request();


    // send video first
    for (Data::DataVector &frame_data : t_video) {
      video_req.set_data(new AVData(std::move(frame_data), Data::Video));
      p2p->send_package(video_req);
    }

    std::this_thread::sleep_for(50ms); // do it for 10 secs then quit.
                                     //
    //  send audio
    audio_req.set_data(new AVData(std::move(t_audio), Data::Audio));
    p2p->send_package(audio_req);
  };
}


void write_wav(){

  wav_header_t header;
  header.riff_header[0] = 'R';
  header.riff_header[1] = 'I';
  header.riff_header[2] = 'F';
  header.riff_header[3] = 'F';
  header.wave_header[0] = 'W';
  header.wave_header[1] = 'A';
  header.wave_header[2] = 'V';
  header.wave_header[3] = 'E';
  header.fmt_header[0] = 'f';
  header.fmt_header[1] = 'm';
  header.fmt_header[2] = 't';
  header.fmt_header[3] = ' ';
  header.fmt_chunk_size = 16;
  header.audio_format = 1;
  header.num_channels = 1;
  header.sample_rate = 44100;
  header.byte_rate = header.sample_rate * header.num_channels * 2; // 2 is byte per sample - 16 bits
  header.sample_alignment = header.num_channels * 2; // 2 is byte per sample
  header.bit_depth = 16;
  header.data_header[0] = 'd';
  header.data_header[1] = 'a';
  header.data_header[2] = 't';
  header.data_header[3] = 'a';

  SDL_Init(SDL_INIT_AUDIO);
  auto input_queue = std::make_unique<LockFreeAudioQueue>();
  AudioDevice input_device(input_queue, AudioDevice::Input);

  input_device.open();
  AudioDevice::wait(100); // record for 200 frames!
  input_device.close();

   std::vector<uint8_t> raw_audio;

  while (!input_queue->empty()) {

    auto ptr = input_queue->pop();
    Data::DataVector data =  ptr->m_data;
    for(auto &d : data){
      raw_audio.push_back(d);
    }
  }

   header.data_bytes = raw_audio.size();
   header.wav_size =   36 + header.data_bytes;

   std::FILE *wave_file;
   wave_file = fopen( "skype.wav" , "a");
   fwrite(&header, sizeof(header), 1, wave_file);
   printf("size: %d\n", header.wav_size);
   fwrite(raw_audio.data(), 1, raw_audio.size(), wave_file);
   fclose(wave_file);
}

/* Tests Audio
 *  ./build/bin/p2p_test audio -
 * */

void test_audio() {
  SDL_Init(SDL_INIT_AUDIO);

  auto input_queue = std::make_unique<LockFreeAudioQueue>();
  auto output_queue = std::make_unique<LockFreeAudioQueue>();

  AudioDevice input_device(input_queue, AudioDevice::Input);
  AudioDevice output_device(output_queue, AudioDevice::Output);

  auto converter = AudioConverter();
  auto audio_bank = AudioBank();

  Data::DataVector to_file;

  std::cout << "Recording. Say something.\n";

  input_device.open();
  AudioDevice::wait(100); // record for 200 frames!
  input_device.close();

  std::cout << "Converting audio...\n";

  while (!input_queue->empty()) {

    if (!converter.valid()) {
      std::cout << "Converter not valid\n";
      break;
    }

      std::vector<uint8_t> encoded_audio = converter.encode(input_queue);
      Data::DataVector decoded_data = converter.decode(encoded_audio);
      AudioPackage audio_pkt(std::move(decoded_data));
      output_queue->push(std::move(audio_pkt));
  }

  std::cout << "done with input! Now playing the ouput of the recorded audio.\n";
  output_device.open();
  AudioDevice::wait(100);
  std::cout << "Done playing back.\n";
  output_device.close();

  SDL_Quit();
  free_configs();
}

/* Tests camera
 *  ./build/bin/p2p_test camera -
 * */

void test_video() {
  auto webcam = Webcam();
  int index = 0;

  while (index < 30) {
    auto res = webcam.capture();

    if (!webcam.valid()) {
      break;
    }

    index++;
  }

  free_configs();
}

/* NOTE: Tests UDP connection
 *       for accepting client
 *          ./build/bin/p2p_test shakira 1234
 *       for connecting client
 *          ./build/bin/p2p_test john 1234
 */

P2P test_conn(char *t_user, char *t_pw) {
  std::string user(t_user);

  ActiveConn conn(config->get<int>("TCP_PORT"), new TextIO());
  std::string token = login_as(conn, t_user, t_pw); // conn , user, password.
                                                    //
  std::cout << token << std::endl;

  P2P p2p(token);

  if (user == "john") { // john connects
    connect_to(p2p);
  } else if (user == "shakira") { // shakira accepts
    accept_from(p2p);
  } else {
    std::cout << "Please pass in 'john' or 'shakira' as users.\n";
  }

  p2p.handshake_peer();

  logout(conn, token);

  Config::free_instance();

  return p2p;
}

/** HELPERS ***/

auto login_as(ActiveConn &conn, const char *user, const char *password)
    -> std::string {

  auto addr = config->get<const std::string>("SERVER_ADDRESS");
  Request req = conn.connect_socket(addr);

  if (!req.m_valid) {
    std::cout << "YOU MUST HAVE THE SERVER RUNNING TO RUN THIS TEST."
              << std::endl;
    return {};
  }

  conn.receive(req); // ignore handshake
  req.set_data(new TextData(std::string("LOGIN ") + user + " " + password));

  conn.respond(req);
  conn.receive(req);

  std::string response = TextData::to_string(req.data());

  auto [_, token] = StringUtils::split_first(response);

  return token;
}

/* */

void logout(ActiveConn &conn, std::string &token) {
  Request req;
  req.m_valid = true;
  req.set_data(new TextData(std::string("EXIT") + " " + token));
  conn.respond(req);
}

/* */

void connect_to(P2P &p2p) {

  std::string user_id = "3";
  p2p.connect_peer(user_id);

  if (p2p.status() != P2P::Awaiting) {
    std::cout << "did not call connect correctly. Exiting...\n";

    return;
  }

  while (p2p.status() != P2P::Accepted) {

    int count = 0;
    std::cout << "Pinging ...\n";

    p2p.ping_peer();
    sleep(2); // check every 2 second

    if (p2p.status() == P2P::Awaiting) {
      std::cout << "Still Awaiting ...\n";
    }
    if (p2p.status() == P2P::Error) {
      std::cout << "ping returned an error. exiting....\n";
      return;
    }

    if (count > 10) {
      std::cout << std::string("Breaking after ") + std::to_string(count)
                << std::endl;
      p2p.hangup_peer();
      return;
    }
    count++;
  }

  std::cout << "connect Accepted sucessfully!\n";
}

/* */

void accept_from(P2P &p2p) {

  std::string user_id = "1";

  p2p.accept_peer(user_id);

  if (p2p.status() == P2P::Accepted) {
    std::cout << "Accepted was sucessful\n";
  } else {

    std::cout << "Accept failed.\n";
  }
}

/* */

void free_configs() {
  VideoSettings::delete_instance();
  AudioSettings::delete_instance();
  AudioDevConfig::delete_instance();
}
