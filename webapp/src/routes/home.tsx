import { h, Fragment } from "preact";
import axios from "axios";
import QrReader from "react-web-qr-reader";
import {
  Button,
  Paper,
  Slider,
  Stack,
  Typography,
  Fab,
  Dialog,
  Table,
  TableBody,
  TableCell,
  TableContainer,
  TableHead,
  TableRow,
  Tooltip,
  IconButton,
} from "@mui/material";
import { ArrowRight, DoubleArrow, QrCode } from "@mui/icons-material";
import { useState, useEffect } from "preact/hooks";
import { Link, route } from "preact-router";

import Station from "../models/station";

const Home = () => {
  const [brightness, setBrightness] = useState<number>(0);
  const [open, setOpen] = useState(false);

  const [stations, setStations] = useState<Station[]>([]);

  const handleScan = (scan) => {
    if (scan) {
      const stationId = parseInt(scan.data, 10);
      if (isNaN(stationId)) setOpen(false);
      else route(`/station/${stationId}`);
    }
  };

  const handleError = (error) => {
    console.log(error);
  };

  const override = async () => {
    await axios.post("http://localhost:8000/on", { brightness, id: 0 });
  };

  const removeOverride = async () => {
    await axios.post("http://localhost:8000/off", { id: 0 });
  };

  useEffect(() => {
    const fetchStations = async () => {
      const { data } = await axios.get("http://localhost:8000/stations");
      setStations(data.stations);
    };

    fetchStations().catch(console.error);
  }, []);

  return (
    <>
      <Fab
        size="large"
        color="secondary"
        onClick={() => setOpen(true)}
        style={{ position: "fixed", bottom: 30, right: 30, zIndex: 1000 }}
      >
        <QrCode />
      </Fab>

      <Typography variant="h2" textAlign="center" marginY={2}>
        Station List
      </Typography>

      <TableContainer component={Paper}>
        <Table aria-label="simple table">
          <TableHead>
            <TableRow>
              <TableCell>Station ID</TableCell>
              <TableCell align="right">Position</TableCell>
              <TableCell align="right">Brightness Level</TableCell>
              <TableCell align="right">Locked</TableCell>
              <TableCell align="right">Actions</TableCell>
            </TableRow>
          </TableHead>
          <TableBody>
            {stations.map((station) => (
              <TableRow
                key={station.node_id}
                sx={{ "&:last-child td, &:last-child th": { border: 0 } }}
              >
                <TableCell component="th" scope="row">
                  {station.node_id}
                </TableCell>
                <TableCell align="right">
                  {`(${station.coord_x},${station.coord_y})`}
                </TableCell>
                <TableCell align="right">{station.brightness}</TableCell>
                <TableCell align="right">
                  {station.locked ? "Yes" : "No"}
                </TableCell>
                <TableCell align="right">
                  <Tooltip title="Open Station">
                    <IconButton
                      onClick={() => route(`/station/${station.node_id}`)}
                    >
                      <DoubleArrow />
                    </IconButton>
                  </Tooltip>
                </TableCell>
              </TableRow>
            ))}
          </TableBody>
        </Table>
      </TableContainer>

      <Typography
        variant="h2"
        textAlign="center"
        marginBottom={2}
        marginTop={6}
      >
        Station Brightness Control
      </Typography>
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

      <Dialog fullWidth={true} open={open} onClose={() => setOpen(false)}>
        <Typography variant="h4" textAlign="center" marginY={2}>
          Station Reader
        </Typography>
        <QrReader delay={500} onError={handleError} onScan={handleScan} />
      </Dialog>
    </>
  );
};

export default Home;
