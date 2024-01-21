#include "quoll/core/Base.h"
#include "quoll/profiler/MetricsCollector.h"
#include "quoll/rhi-mock/MockRenderDevice.h"
#include "quoll-tests/Testing.h"

class MetricsCollectorTest : public ::testing::Test {
public:
  quoll::rhi::MockRenderDevice device;
  quoll::MetricsCollector collector;
};

using MetricsCollectorDeathTest = MetricsCollectorTest;

TEST_F(MetricsCollectorTest, CreatesGpuSpanWithLabelAndMarks) {
  {
    auto spanView = collector.createGpuSpan("Span 1");
    EXPECT_EQ(spanView.getIndex(), 0);
    EXPECT_EQ(spanView.getStartMark(), 0);
    EXPECT_EQ(spanView.getEndMark(), 1);

    auto span = collector.getGpuSpanData(0);
    EXPECT_EQ(span.label, "Span 1");
    EXPECT_EQ(span.markStart, 0);
    EXPECT_EQ(span.markEnd, 1);
  }

  {
    auto spanView = collector.createGpuSpan("Span 2");
    EXPECT_EQ(spanView.getIndex(), 1);
    EXPECT_EQ(spanView.getStartMark(), 2);
    EXPECT_EQ(spanView.getEndMark(), 3);

    auto span = collector.getGpuSpanData(1);
    EXPECT_EQ(span.label, "Span 2");
    EXPECT_EQ(span.markStart, 2);
    EXPECT_EQ(span.markEnd, 3);
  }
}

TEST_F(MetricsCollectorTest, CreatingSpansInitializesQueriesForEachSpan) {
  auto span1 = collector.createGpuSpan("Span 1");
  auto span2 = collector.createGpuSpan("Span 2");
  auto span3 = collector.createGpuSpan("Span 3");

  EXPECT_EQ(collector.getNumQueries(), 6);
  EXPECT_EQ(collector.getQueryForMark(span1.getStartMark()), 0);
  EXPECT_EQ(collector.getQueryForMark(span1.getEndMark()), 0);
  EXPECT_EQ(collector.getQueryForMark(span2.getStartMark()), 0);
  EXPECT_EQ(collector.getQueryForMark(span2.getEndMark()), 0);
  EXPECT_EQ(collector.getQueryForMark(span3.getStartMark()), 0);
  EXPECT_EQ(collector.getQueryForMark(span3.getEndMark()), 0);
}

TEST_F(MetricsCollectorDeathTest,
       DeletingGpuSpanGetsRidOfTheSpanMarksAndTimestamps) {
  auto span1 = collector.createGpuSpan("Span 1");
  auto span2 = collector.createGpuSpan("Span 2");

  collector.deleteGpuSpan(span2);

  EXPECT_DEATH(collector.getGpuSpanData(1), ".*");
  EXPECT_DEATH(collector.getQueryForMark(span2.getStartMark()), ".*");
  EXPECT_DEATH(collector.getQueryForMark(span2.getEndMark()), ".*");
}

TEST_F(MetricsCollectorTest, DeletingSpanDeletesQueriesAssociatedWithSpan) {
  auto span1 = collector.createGpuSpan("Span 1");
  auto span2 = collector.createGpuSpan("Span 2");
  auto span3 = collector.createGpuSpan("Span 3");
  auto span4 = collector.createGpuSpan("Span 4");
  auto span5 = collector.createGpuSpan("Span 5");
  auto span6 = collector.createGpuSpan("Span 6");

  EXPECT_EQ(collector.getNumQueries(), 12);

  collector.deleteGpuSpan(span3);

  EXPECT_EQ(collector.getNumQueries(), 10);
  EXPECT_EQ(collector.getQueryForMark(span1.getStartMark()), 0);
  EXPECT_EQ(collector.getQueryForMark(span1.getEndMark()), 0);
  EXPECT_EQ(collector.getQueryForMark(span2.getStartMark()), 0);
  EXPECT_EQ(collector.getQueryForMark(span2.getEndMark()), 0);
  EXPECT_EQ(collector.getQueryForMark(span4.getStartMark()), 0);
  EXPECT_EQ(collector.getQueryForMark(span4.getEndMark()), 0);
  EXPECT_EQ(collector.getQueryForMark(span5.getStartMark()), 0);
  EXPECT_EQ(collector.getQueryForMark(span5.getEndMark()), 0);
  EXPECT_EQ(collector.getQueryForMark(span6.getStartMark()), 0);
  EXPECT_EQ(collector.getQueryForMark(span6.getEndMark()), 0);
}

TEST_F(MetricsCollectorTest, CreatingSpansDoesNotAffectTimestamps) {
  auto span1 = collector.createGpuSpan("Span 1");
  auto span2 = collector.createGpuSpan("Span 2");
  auto span3 = collector.createGpuSpan("Span 3");

  EXPECT_EQ(collector.getNumCollectedTimestamps(), 0);
}

TEST_F(MetricsCollectorTest, DeletingSpansDoesNotAffectTimestamps) {
  auto span1 = collector.createGpuSpan("Span 1");
  auto span2 = collector.createGpuSpan("Span 2");
  auto span3 = collector.createGpuSpan("Span 3");

  collector.deleteGpuSpan(span2);

  EXPECT_EQ(collector.getNumCollectedTimestamps(), 0);
}

TEST_F(MetricsCollectorTest,
       GetResultsDoesNothingIfNoSpanHasWrittenTimestamps) {
  device.setTimestampCollectorFn([](auto &timestamps) {
    for (usize i = 0; i < timestamps.size(); ++i) {
      timestamps.at(i) = (i + 2) * (i + 2);
    }
  });

  auto span1 = collector.createGpuSpan("Span 1");
  auto span2 = collector.createGpuSpan("Span 2");
  auto span3 = collector.createGpuSpan("Span 3");

  collector.getResults(&device);

  EXPECT_EQ(collector.getNumCollectedTimestamps(), 0);
}

TEST_F(MetricsCollectorTest, GpuSpanBeginCallsTimestampWithBeginQueryIndex) {
  auto commandList = device.requestImmediateCommandList();

  auto span1 = collector.createGpuSpan("Span 1");
  auto span2 = collector.createGpuSpan("Span 2");

  span1.begin(commandList);
  span2.begin(commandList);

  const auto &commands = static_cast<const quoll::rhi::MockCommandList *>(
                             commandList.getNativeRenderCommandList().get())
                             ->getCommands();

  EXPECT_EQ(commands.size(), 2);

  auto *cmd0 =
      static_cast<quoll::rhi::MockCommandTimestamp *>(commands.at(0).get());
  auto *cmd1 =
      static_cast<quoll::rhi::MockCommandTimestamp *>(commands.at(1).get());

  EXPECT_EQ(cmd0->queryIndex, 0);
  EXPECT_EQ(cmd0->stage, quoll::rhi::PipelineStage::PipeTop);
  EXPECT_EQ(cmd1->queryIndex, 1);
  EXPECT_EQ(cmd1->stage, quoll::rhi::PipelineStage::PipeTop);
}

TEST_F(MetricsCollectorTest, GpuSpanEndCallsTimestampWithEndQueryIndex) {
  auto commandList = device.requestImmediateCommandList();

  auto span1 = collector.createGpuSpan("Span 1");
  auto span2 = collector.createGpuSpan("Span 2");

  span1.end(commandList);
  span2.end(commandList);

  const auto &commands = static_cast<const quoll::rhi::MockCommandList *>(
                             commandList.getNativeRenderCommandList().get())
                             ->getCommands();

  EXPECT_EQ(commands.size(), 2);

  auto *cmd0 =
      static_cast<quoll::rhi::MockCommandTimestamp *>(commands.at(0).get());
  auto *cmd1 =
      static_cast<quoll::rhi::MockCommandTimestamp *>(commands.at(1).get());

  EXPECT_EQ(cmd0->queryIndex, 0);
  EXPECT_EQ(cmd0->stage, quoll::rhi::PipelineStage::PipeBottom);
  EXPECT_EQ(cmd1->queryIndex, 1);
  EXPECT_EQ(cmd1->stage, quoll::rhi::PipelineStage::PipeBottom);
}

TEST_F(MetricsCollectorTest,
       GetResultProvidesTimestampsForSpansThatHaveWrittenTimestamps) {
  auto commandList = device.requestImmediateCommandList();

  device.setTimestampCollectorFn([](auto &timestamps) {
    for (usize i = 0; i < timestamps.size(); ++i) {
      timestamps.at(i) = (i + 2) * (i + 2);
    }
  });

  auto span1 = collector.createGpuSpan("Span 1");
  auto span2 = collector.createGpuSpan("Span 2");
  auto span3 = collector.createGpuSpan("Span 3");

  span1.begin(commandList);
  span3.end(commandList);

  collector.markForCollection();
  collector.getResults(&device);

  EXPECT_EQ(collector.getNumCollectedTimestamps(), 2);
}

TEST_F(MetricsCollectorTest,
       SubsequentTimestampWritesIncreasesNumberOfTimestamps) {
  auto commandList = device.requestImmediateCommandList();

  auto span1 = collector.createGpuSpan("Span 1");
  auto span2 = collector.createGpuSpan("Span 2");
  auto span3 = collector.createGpuSpan("Span 3");

  span1.begin(commandList);
  span3.end(commandList);

  span1.begin(commandList);
  span3.end(commandList);

  collector.markForCollection();
  collector.getResults(&device);

  EXPECT_EQ(collector.getNumCollectedTimestamps(), 4);
}

TEST_F(MetricsCollectorTest, GetResultClearsRecordedTimestamps) {
  auto commandList = device.requestImmediateCommandList();

  auto span1 = collector.createGpuSpan("Span 1");
  auto span2 = collector.createGpuSpan("Span 2");
  auto span3 = collector.createGpuSpan("Span 3");

  span1.begin(commandList);
  span3.end(commandList);

  collector.markForCollection();
  collector.getResults(&device);

  span1.begin(commandList);
  span3.end(commandList);

  EXPECT_EQ(collector.getNumCollectedTimestamps(), 2);
}

TEST_F(MetricsCollectorTest, GetResultsDoesNothingIfNotMarkedForCollection) {
  auto commandList = device.requestImmediateCommandList();

  auto span1 = collector.createGpuSpan("Span 1");
  auto span2 = collector.createGpuSpan("Span 2");
  auto span3 = collector.createGpuSpan("Span 3");

  span1.begin(commandList);
  span3.end(commandList);

  collector.getResults(&device);

  span1.begin(commandList);
  span3.end(commandList);

  EXPECT_EQ(collector.getNumCollectedTimestamps(), 0);
}

TEST_F(MetricsCollectorTest, MeasureCalculatesSpanDurationsBasedOnTimestamps) {
  auto commandList = device.requestImmediateCommandList();

  device.setTimestampCollectorFn([](auto &timestamps) {
    for (usize i = 0; i < timestamps.size(); ++i) {
      timestamps.at(i) = (i + 2) * (i + 2);
    }
  });

  auto span1 = collector.createGpuSpan("Span 1");
  auto span2 = collector.createGpuSpan("Span 2");
  auto span3 = collector.createGpuSpan("Span 3");

  span1.begin(commandList);
  span1.end(commandList);
  span2.begin(commandList);
  span2.end(commandList);
  span3.begin(commandList);
  span3.end(commandList);

  collector.markForCollection();
  collector.getResults(&device);

  auto metrics = collector.measure(2.5f);

  EXPECT_EQ(metrics.size(), 3);
  EXPECT_EQ(metrics.at(0).label, "Span 1");
  EXPECT_EQ(metrics.at(1).label, "Span 2");
  EXPECT_EQ(metrics.at(2).label, "Span 3");

  EXPECT_EQ(metrics.at(0).value, 12.5f);
  EXPECT_EQ(metrics.at(1).value, 22.5f);
  EXPECT_EQ(metrics.at(2).value, 32.5f);
}

TEST_F(MetricsCollectorTest, MeasureIgnoresSpansThatHaveNotWrittenTimestamps) {
  auto commandList = device.requestImmediateCommandList();

  device.setTimestampCollectorFn([](auto &timestamps) {
    for (usize i = 0; i < timestamps.size(); ++i) {
      timestamps.at(i) = (i + 2) * (i + 2);
    }
  });

  auto span1 = collector.createGpuSpan("Span 1");
  auto span2 = collector.createGpuSpan("Span 2");
  auto span3 = collector.createGpuSpan("Span 3");

  span1.begin(commandList);
  span1.end(commandList);
  span3.begin(commandList);
  span3.end(commandList);

  collector.markForCollection();
  collector.getResults(&device);

  auto metrics = collector.measure(2.5f);

  EXPECT_EQ(metrics.size(), 2);
  EXPECT_EQ(metrics.at(0).label, "Span 1");
  EXPECT_EQ(metrics.at(1).label, "Span 3");

  EXPECT_EQ(metrics.at(0).value, 12.5f);
  EXPECT_EQ(metrics.at(1).value, 22.5f);
}
