#pragma once

#include "config.h"

#if USE_AZURE_BLOB_STORAGE

#include <Storages/StorageAzureBlobCluster.h>
#include <Storages/StorageAzureBlob.h>
#include <Interpreters/threadPoolCallbackRunner.h>
#include <base/types.h>
#include <functional>
#include <memory>


namespace DB
{
class SeekableReadBuffer;

using CreateReadBuffer = std::function<std::unique_ptr<SeekableReadBuffer>()>;

/// Copies a file from AzureBlobStorage to AzureBlobStorage.
/// The parameters `src_offset` and `src_size` specify a part in the source to copy.
void copyAzureBlobStorageFile(
    MultiVersion<Azure::Storage::Blobs::BlobContainerClient> & src_client,
    MultiVersion<Azure::Storage::Blobs::BlobContainerClient> & dest_client,
    const String & src_container_for_logging,
    const String & src_blob,
    size_t src_offset,
    size_t src_size,
    const String & dest_container_for_logging,
    const String & dest_blob,
    MultiVersion<AzureObjectStorageSettings> settings,
    const ReadSettings & read_settings,
    ThreadPoolCallbackRunner<void> schedule_ = {},
    bool for_disk_azure_blob_storage = false);


/// Copies data from any seekable source to AzureBlobStorage.
/// The same functionality can be done by using the function copyData() and the class WriteBufferFromS3
/// however copyDataToS3File() is faster and spends less memory.
/// The callback `create_read_buffer` can be called from multiple threads in parallel, so that should be thread-safe.
/// The parameters `offset` and `size` specify a part in the source to copy.
void copyDataToAzureBlobStorageFile(
    const std::function<std::unique_ptr<SeekableReadBuffer>()> & create_read_buffer,
    size_t offset,
    size_t size,
    MultiVersion<Azure::Storage::Blobs::BlobContainerClient> & client,
    const String & dest_container_for_logging,
    const String & dest_blob,
    MultiVersion<AzureObjectStorageSettings> settings,
    ThreadPoolCallbackRunner<void> schedule_ = {},
    bool for_disk_azure_blob_storage = false);

}

#endif
