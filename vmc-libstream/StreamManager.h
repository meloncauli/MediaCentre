#ifndef STREAMMANAGER_H
#define STREAMMANAGER_H

#include <memory>
#include <mutex>

#include <vmc-libutils/LockedResource.h>

#include "Stream.h"

namespace vmc
{
    namespace stream
    {
        class StreamManager
        {
        public:
            StreamManager();
            void play(std::unique_ptr<Stream> stream);

            void lockStream();
            LockedResource<Stream> getStream();
            void freeStream();

        private:
            std::unique_ptr<Stream> currentStream;
            std::mutex streamMutex;
        };
    }
}

#endif
