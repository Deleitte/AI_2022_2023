import { h, Fragment } from "preact";
import axios from "axios";
import { Button, Paper, Slider, Stack, Typography } from "@mui/material";
import { useState } from "preact/hooks";
import {
  CartesianGrid,
  Line,
  LineChart,
  ResponsiveContainer,
  Tooltip,
  XAxis,
  YAxis,
} from "recharts";
import { Box } from "@mui/system";

const fetchedData = [
  {
    timestamp: 1679502886893,
    brightness: 100,
  },
  {
    timestamp: 1679502901566,
    brightness: 0,
  },
  {
    timestamp: 1679502936610,
    brightness: 25,
  },
  {
    timestamp: 1679502955022,
    brightness: 50,
  },
  {
    timestamp: 1679503095266,
    brightness: 100,
  },
  {
    timestamp: 1679503095276,
    brightness: 75,
  },
  {
    timestamp: 1679503095286,
    brightness: 25,
  },
  {
    timestamp: 1679503095296,
    brightness: 50,
  },
];

const Home = () => {
  const [brightness, setBrightness] = useState<number>(0);
  const id = "0";

  const override = async () => {
    await axios.post("http://localhost:8000/on", { brightness, id });
  };

  const removeOverride = async () => {
    await axios.post("http://localhost:8000/off", { id });
  };

  const data = fetchedData.map((d) => ({
    timestamp: new Date(d.timestamp).toLocaleString(),
    brightness: d.brightness,
  }));

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
              <XAxis dataKey="timestamp" />
              <YAxis dataKey="brightness" />
              <Tooltip />
              <CartesianGrid stroke="#eee" strokeDasharray="5 5" />
              <Line type="stepAfter" dataKey="brightness" data={data} />
            </LineChart>
          </ResponsiveContainer>
        </Box>
      </Paper>
    </>
  );
};

export default Home;
