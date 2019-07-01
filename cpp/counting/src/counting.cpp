#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <vector>
#include <tuple>

struct CommonCtx {
    uint8_t printerTid_;
    uint8_t curCount_;
    bool curCountHandled_;

    uint8_t totalCount_;
    bool doDebug_;

    std::mutex m_;
    std::condition_variable cv_;

    explicit CommonCtx(bool doDebug, uint8_t totalCount) :
        totalCount_(totalCount), doDebug_(doDebug) {}
    CommonCtx(const CommonCtx &) = delete;
    CommonCtx & operator=(const CommonCtx&) = delete;
    CommonCtx(CommonCtx &&) = delete;
    CommonCtx & operator=(CommonCtx&&) = delete;
};

struct ThreadCtx {
    uint8_t threadId_;
    CommonCtx &commonCtx_;

    explicit ThreadCtx(CommonCtx &ctx, uint8_t threadId) :
        threadId_(threadId), commonCtx_(ctx) {}
};

static void
countingFn(std::shared_ptr<ThreadCtx> ctx)
{
}

int
main(int argc, char **argv)
{
    const auto threadCount = 5;
    const auto totalCount = 50;
    const auto doDebug = false;

    CommonCtx ctx(doDebug, totalCount);
    //std::vector<std::tuple<std::thread, ThreadCtx>> threadVec;
    std::vector<std::thread> threadVec;

    for (auto i = 0; i < threadCount; ++i) {
        auto tCtx = std::make_shared<ThreadCtx>(ctx, (i + 1));
#if 0
        threadVec.push_back(std::make_tuple(std::thread(countingFn, tCtx),
                                            tCtx));
#endif
        threadVec.push_back(std::thread(countingFn, tCtx));
    }

    auto &tid = ctx.printerTid_;
    {
        std::unique_lock<std::mutex> lk(ctx.m_);
        tid = 0;
        for (ctx.curCount_ = 1; ctx.curCount_ <= ctx.totalCount_;
             ++(ctx.curCount_)) {
            tid = (tid >= threadCount) ?  1 : (tid + 1);
            ctx.curCountHandled_ = false;
            std::cout << __func__ << ": curCount=" << ctx.curCount_ <<
                std::endl;
            ctx.cv_.notify_all();
            ctx.cv_.wait(lk, [&]{ return ctx.curCountHandled_; });
        }
    }

    for (auto &it : threadVec) {
        //std::get<0>(it).join();
        it.join();
    }

    return 0;
}
