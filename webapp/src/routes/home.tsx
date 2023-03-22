import { h, Fragment } from "preact";
import axios from "axios";
import { Button, Paper, Slider, Stack, Typography } from "@mui/material";
import { useState } from "preact/hooks";
import {
  CartesianGrid,
  Line,
  LineChart,
  ResponsiveContainer,
  XAxis,
  YAxis,
} from "recharts";
import { Box } from "@mui/system";

const data = [
  {
    name: "Station 1",
    data: [
      {
        timestamp: 1,
        brightness: 50,
      },
      {
        timestamp: 5,
        brightness: 0,
      },
      {
        timestamp: 10,
        brightness: 50,
      },
    ],
  },
  {
    name: "Station 2",
    data: [
      {
        timestamp: 1,
        brightness: 75,
      },
      {
        timestamp: 2,
        brightness: 50,
      },
      {
        timestamp: 5,
        brightness: 100,
      },
    ],
  },
  {
    name: "Station 3",
    data: [
      {
        timestamp: 1,
        brightness: 0,
      },
      {
        timestamp: 2,
        brightness: 50,
      },
      {
        timestamp: 7,
        brightness: 25,
      },
    ],
  },
];

const getRandomColor = () => {
  const letters = "0123456789ABCDEF";
  let color = "#";
  for (let i = 0; i < 6; i++) {
    color += letters[Math.floor(Math.random() * 16)];
  }
  return color;
};

const Home = () => {
  const [brightness, setBrightness] = useState<number>(0);

  const override = async () => {
    await axios.post("http://localhost:8000/on", { brightness });
  };

  const removeOverride = async () => {
    await axios.post("http://localhost:8000/off");
  };

  return (
    <>
      <Paper elevation={3}>
        <Stack spacing={4} sx={{ mb: 1 }} padding={6} textAlign="center">
          <Typography variant="h4">
            Brightness Override: {brightness}%
          </Typography>
          <Stack spacing={2} direction="row" alignItems="center">
            <Slider
              aria-label="Override brightness"
              defaultValue={0}
              step={25}
              marks
              min={0}
              max={100}
              valueLabelDisplay="auto"
              onChange={(_, value) => setBrightness(value as number)}
            />

            <Button variant="contained" onClick={() => override()}>
              Override Brightness
            </Button>
          </Stack>

          <Button
            variant="contained"
            color="secondary"
            onClick={() => removeOverride()}
          >
            Remove override
          </Button>
        </Stack>
      </Paper>

      <Paper elevation={3}>
        <Box textAlign="center">
          <Typography variant="h4" marginY={4}>
            Brightness History
          </Typography>
          <ResponsiveContainer width="100%" height={300}>
            <LineChart
              width={500}
              height={300}
              data={data}
              margin={{
                top: 5,
                right: 30,
                left: 20,
                bottom: 5,
              }}
            >
              <XAxis dataKey="timestamp" type="number" />
              <YAxis dataKey="brightness" />
              <CartesianGrid stroke="#eee" strokeDasharray="5 5" />
              {data.map((s) => (
            <Line type="monotone" dataKey="brightness" data={s.data} name={s.name} key={s.name} stroke={getRandomColor()} />
          ))}
            </LineChart>
          </ResponsiveContainer>
        </Box>
      </Paper>
    </>
  );
};

export default Home;
