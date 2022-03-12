#include <sys/unistd.h>

#include "layer_definitions.h"
#include "fd_wraper.h"
#include "factory.h"
#include "buffer_layer.h"
#include "cipher_layer.h"
#include "api_message_layer.h"
#include "api.h"

int main() {
    int pipe_fd[2];
    TERMINATE_IF(pipe(pipe_fd), "Failed to make pipe", ERR_ERRNO);
    fd_resource_t *fd_resource = fd_resource_make(pipe_fd[0], pipe_fd[1]);

    // Although I didn't use the args, this demonstrates how we could enclose some arguments to use at make-time
    layer_factory_t *fd_layer_factory = make_factory(fd_layer_make, NULL);
    layer_factory_t *buffer_layer_factory = make_factory(buffer_layer_make, NULL);
    layer_factory_t *cipher_layer_factory = make_factory(cipher_layer_make, NULL);
    layer_factory_t *api_message_layer_factory = make_factory(api_message_layer_make, NULL);
    // The api factory actually points to the bottom-most layer
    // When creating a stack, you actually get a handle to the top-most layer. The api in this case.
    layer_factory_t *api_factory = fd_layer_factory
            ->set_next_factory(fd_layer_factory, buffer_layer_factory)
            ->set_next_factory(fd_layer_factory, cipher_layer_factory)
            ->set_next_factory(fd_layer_factory, api_message_layer_factory)
            ->set_next_factory(fd_layer_factory, make_factory(api_make, NULL));
    api_t *api = (api_t *) api_factory->make_with(api_factory, fd_resource);
    api->say_hi(api);
    run_event_loop();
}
